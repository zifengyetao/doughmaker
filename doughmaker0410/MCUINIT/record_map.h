 
#define EEPROM_SIZE					0x80000  //64KB  
#define START_EEADR        0x00000  //ҳ��һ��ʼ��ַ

/*
 #define PAGE5_WAKEUP3_EEADR   EEPROM_SIZE-PAGE4_BYTE_NUM//������
 #define PAGE5_MIXFLOUR_FAST 			PAGE5_WAKEUP3_EEADR-PAGE5_BYTE_NUM//���ٺ���
 #define PAGE5_WAKEUP2_EEADR   PAGE5_MIXFLOUR_FAST-PAGE4_BYTE_NUM//�����
 #define PAGE5_MIXFLOUR_MID         PAGE5_WAKEUP2_EEADR-PAGE5_BYTE_NUM//���ٺ���
 #define PAGE5_WAKEUP1_EEADR   PAGE5_MIXFLOUR_MID-PAGE4_BYTE_NUM//����һ
 #define PAGE5_MIXFLOUR_SLOW      PAGE5_WAKEUP1_EEADR-PAGE5_BYTE_NUM//���ٺ�����ʼ��ַ
 
 
 #define Page1_start_pointer   								   PAGE1_EEADR   //ҳ��1��ʼ��ַ
 #define EE_Flag_pointer 				              	Page1_start_pointer+11//EEPROM��д��¼flag
 
 #define Page5_nodles_slowmix_start_pointer   PAGE5_MIXFLOUR_SLOW
 #define Page5_bun_slowmix_start_pointer        Page5_nodles_slowmix_start_pointer-21
 #define Page5_dump_slowmix_start_pointer     Page5_bun_slowmix_start_pointer-21
 */
 
#define EE_Flag_pointer 				              	START_EEADR+11//EEPROM��д��¼flag

#define Setting_Size 10
#define Hand_Size 4

#define auto_page_pointer  EEPROM_SIZE -  Setting_Size
#define Hand_page_pointer  auto_page_pointer -  Hand_Size














