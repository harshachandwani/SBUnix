#include <sys/defs.h>
#include <sys/generic.h>
#include <sys/kprintf.h>
#include <sys/ahci.h>
#include <sys/ahci_lib.h>
#include <sys/string.h>

#define AHCI_BASE 0x600000
static int block_id = 0;
uint64_t *buf = (uint64_t*)0x700000;

int write(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count) //, uint32_t buf)
{
	port->is_rwc = (uint32_t)-1	;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	int i;
	uint64_t *temp = buf;

	for(int iter = 0; iter < 4096; iter++){
		*buf = block_id /8;
		buf++;
	}	

	block_id++;
	if (slot == -1)
		return -1;

	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 1;		// Write to device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl - 1)*sizeof(hba_prdt_entry_t));
	 

	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba =(uint64_t)temp;
		cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K bytes
		count -= 16;	// 8 sectors = 4K bytes
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t)temp;
	cmdtbl->prdt_entry[i].dbc = count << 9;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
	
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->count = count;
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return -1;
	}
 
	port->ci = 1<<slot;	// Issue command
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return 1;
		}
	}
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return 1;
	}
 
	return 0;
}

int read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t *buf)
{
	port->is_rwc = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	int i = 0;
	if (slot == -1)
		return -1;
 
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl - 1)*sizeof(hba_prdt_entry_t));
 
	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 4 * 1024;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
	//	buf += 4*1024;	// 4K words
		count -= 8;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->count = count;
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return -1;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return -1;
		}
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return -1;
	}

	return 0;
}
void port_rebase(hba_port_t *port, int portno)
{
	
	stop_cmd(port);	// Stop command engine
 
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno<<10);
	memset((void*)(port->clb), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = AHCI_BASE + (32<<10) + (portno<<8);
	memset((void*)(port->fb), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)(port->clb);
	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
		memset((void*)cmdheader[i].ctba, 0, 256);
	}
	start_cmd(port);	// Start command engine
}
// Start command engine
void start_cmd(hba_port_t *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(hba_port_t *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
}



 
// Find a free command list slot
int find_cmdslot(hba_port_t *port)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (port->sact | port->ci);
	for (int i=0; i< 32; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	kprintf("Cannot find free command list entry\n");
	return -1;
}


uint16_t pciConfigReadWord (uint8_t bus, uint8_t device,uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
	
    /* write out the address */
    outl(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((inl (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}


uint32_t pciConfigWriteWord (uint8_t bus, uint8_t device,uint8_t func, uint8_t offset) {
	uint32_t address;
	uint32_t lbus  = (uint32_t)bus;
	uint32_t ldevice = (uint32_t)device;
	uint32_t lfunc = (uint32_t)func;
	
	/* create configuration address as per Figure 1 */
	address = (uint32_t)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
	//kprintf("%x\n",address);
	outl(0xCF8, address);
    	/* read in the data */
	//if(inl(0xCFC) == 0xFEBF1000)
	outl(0xCFC,0xa6000);
	return ((uint32_t)inl(0xCFC));
}

void checkAHCI(uint8_t bus,uint8_t device, uint8_t fun){
        uint16_t vendorID = pciConfigReadWord(bus,device,fun,0);
	uint16_t class_id;
	//uint16_t abar_lo, abar_hi;
	uint8_t subclass_code, class_code;
	uint64_t abar;
	//uint64_t abar_64;
        if(vendorID != 0xFFFF){
       		class_id = pciConfigReadWord(bus,device,fun,0x0A);
		subclass_code = (uint8_t) (class_id & 0xFF);
		class_code = (uint8_t)((class_id >> 8) &&  0xFF);
		if((subclass_code == 0x06) && (class_code == 0x01)){
			kprintf("AHCI Found!! \n");
			abar = (uint32_t)pciConfigWriteWord(bus, device,fun,0x24);
			kprintf("\nabar = %x",(abar));	
			probe_port((hba_mem_t *)(abar));
		}
        }
}


void probe_port(hba_mem_t *abar)
{
	// Search disk in implemented ports
	
	uint32_t pi = abar->pi;
	int i = 0;
	kprintf("\npi = %d\n", pi);
	
	uint64_t *buff = (uint64_t*)0x800000;
	while (i<32)
	{
		if (pi & 1)
		{
			
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				if(i == 1){	
				abar->ghc = (uint32_t) (1 << 31);
				abar->ghc = (uint32_t) (1 << 0);
				abar->ghc = (uint32_t) (1 << 31);
				abar->ghc = (uint32_t) (1 << 1);
				port_rebase(&abar->ports[i],i);
						
				for(int iter = 0; iter < 100; iter++){
					write(&abar->ports[i],iter * 8 ,0 ,8);
					read(&abar->ports[i], iter * 8, 0, 8, buff);
					buff += 4 * 1024;
				}
				}
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				kprintf("PM drive found at port %d\n", i);
			}
			else
			{
			//	kprintf("No drive found at port %d\n", i);
			}
		}
 
		pi >>= 1;
		i++;
	}
}

int check_type(hba_port_t *port)
{
	/*
	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return 0;
	//	return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return 0;
	//	return AHCI_DEV_NULL;
 */
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

void detect_ahci(){
        int bus;
        int device;
	uint8_t fun;
        for(bus = 0;bus < 256; bus++){
                for(device = 0;device  < 32; device++ ){
			for(fun = 0; fun < 8; fun++) 
                       		checkAHCI(bus,device,fun);
                }
        }

}
