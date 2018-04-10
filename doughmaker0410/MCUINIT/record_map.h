 
 #define PAGE5_BYTE_NUM      4
 #define PAGE4_BYTE_NUM  		3
 #define EEPROM_SIZE					0x80000  //64KB  
 #define PAGE1_EEADR        0x00000  //ҳ��һ��ʼ��ַ

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
 
// #define Page5_nodles_slowmix_start_pointer     	 PAGE5_MIXFLOUR_SLOW//�����Զ��岿�����ٺ����ַ
//  #define Page5_nodles_wakeup1_start_pointer     	 PAGE5_WAKEUP1_EEADR//�����Զ��岿�����ٺ����ַ
// #define Page5_nodles_midmix_start_pointer     	   PAGE5_MIXFLOUR_MID//�����Զ��岿�����ٺ����ַ
//  #define Page5_nodles_wakeup2_start_pointer     	 PAGE5_WAKEUP2_EEADR//�����Զ��岿�����ٺ����ַ
// #define Page5_nodles_fastmix_start_pointer     	   PAGE5_MIXFLOUR_FAST//�����Զ��岿�����ٺ����ַ
// #define Page5_nodles_wakeup3_start_pointer     	 PAGE5_WAKEUP3_EEADR//�����Զ��岿�����ٺ����ַ
// 
//  #define Page5_bun_slowmix_start_pointer     	 PAGE5_MIXFLOUR_SLOW-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*3//�����Զ��岿�����ٺ����ַ
//  #define Page5_bun_wakeup1_start_pointer     	 PAGE5_WAKEUP1_EEADR-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*3// 
// #define Page5_bun_midmix_start_pointer     	   PAGE5_MIXFLOUR_MID-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*3// 
//  #define Page5_bun_wakeup2_start_pointer     	 PAGE5_WAKEUP2_EEADR-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*3// 
// #define Page5_bun_fastmix_start_pointer     	   PAGE5_MIXFLOUR_FAST-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*3// 
// #define Page5_bun_wakeup3_start_pointer     	 PAGE5_WAKEUP3_EEADR-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*3//  
// 
//  #define Page5_dump_slowmix_start_pointer     	 PAGE5_MIXFLOUR_SLOW-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*6//ˮ���Զ��岿�����ٺ����ַ
//  #define Page5_dump_wakeup1_start_pointer     	 PAGE5_WAKEUP1_EEADR-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*6// 
// #define Page5_dump_midmix_start_pointer     	   PAGE5_MIXFLOUR_MID-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*6// 
//  #define Page5_dump_wakeup2_start_pointer     	 PAGE5_WAKEUP2_EEADR-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*6// 
// #define Page5_dump_fastmix_start_pointer     	   PAGE5_MIXFLOUR_FAST-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*6// 
// #define Page5_dump_wakeup3_start_pointer     	 PAGE5_WAKEUP3_EEADR-(PAGE5_BYTE_NUM+PAGE4_BYTE_NUM)*6
