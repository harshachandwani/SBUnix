void detect_ahci();
void checkAHCI(uint8_t bus,uint8_t device,uint8_t fun);
uint16_t pciConfigReadWord (uint8_t bus, uint8_t device,uint8_t func, uint8_t offset);
void probe_port(hba_mem_t *abar);
int check_type(hba_port_t *port);
int find_cmdslot(hba_port_t *port);
void start_cmd(hba_port_t *port);
void stop_cmd(hba_port_t *port);
void port_rebase(hba_port_t *port, int portno);
