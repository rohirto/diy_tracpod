/*
 * treel_tag.c
 *
 *  Created on: Dec 31, 2021
 *      Author: rohirto
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mbedtls/aes.h"
#include "app_config.h"
#include "treel_tag.h"



/* TPMS Variiables */
unsigned char payload_buff[250] = {0};
uint8_t print_buff[250] = {0};
size_t payload_len = 0;
uint8_t treel_ID[2] = {0};
uint8_t macID[6] = {0};
char * aes_key = "#@Trl2018-lespl$";
//char aes_key[16]  = {'#','@','T','r','l','2','0','1','8','-','l','e','s','p','l','$'}
uint8_t data_to_decode[16] = {0};
unsigned char cipherTextOutput[16];
unsigned char decipheredTextOutput[16];
treel_tag_data app_tag;

void decrypt(unsigned char * chipherText, char * key, unsigned char * outputBuffer){

  mbedtls_aes_context aes;

  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_dec( &aes, (const unsigned char*) key, strlen(key) * 8 );
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)chipherText, outputBuffer);
  mbedtls_aes_free( &aes );
}

void filter_data()
{
  /* Get Treel ID */
  treel_ID[0] = payload_buff[6];
  treel_ID[1] = payload_buff[5];

  /* Get Mac ID */
  for( int i = 0; i<6 ; i++)
  {
    macID[i] = payload_buff[9+i];
  }
  /* Get to be decoded data */
  for (int i = 0; i < 16 ; i++)
  {
    data_to_decode[i] = payload_buff[15+i];
  }

}

void process_data()
{
  //AES Decryption
  decrypt(data_to_decode, aes_key, decipheredTextOutput);

  /* Extract Temperature */
  app_tag.tag_temperature = ((uint16_t)decipheredTextOutput[2] <<8)|decipheredTextOutput[1];


  /* Extract Pressure */
  app_tag.tag_pressure = ((uint16_t)decipheredTextOutput[4] <<8)|decipheredTextOutput[3];

  /* Battery Percentage */
  app_tag.tag_battery = decipheredTextOutput[5];

  /* Event Flags */
  app_tag.tag_eventFlags = decipheredTextOutput[6];

  /* Tire Tamper Count */
  app_tag.tag_tamperCount = decipheredTextOutput[7];

  /* X axis and Z axis Offset */
  app_tag.tag_Xoffset = decipheredTextOutput[8];
  app_tag.tag_Zoffset = decipheredTextOutput[9];

  /* Raw vibrations on X and Z */
  app_tag.tag_Xvibration = ((uint16_t)decipheredTextOutput[11] <<8)|decipheredTextOutput[10];
  app_tag.tag_Zvibration = ((uint16_t)decipheredTextOutput[13] <<8)|decipheredTextOutput[12];

  /* Internal Temperature and FW version */
  app_tag.tag_FWversion = ((uint16_t)decipheredTextOutput[15] <<8)|decipheredTextOutput[14];

}


