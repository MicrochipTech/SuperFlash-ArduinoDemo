/*
 * © 2022 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software and any
 * derivatives exclusively with Microchip products. It is your responsibility to comply
 * with third party license terms applicable to your use of third party software
 * (including open source software) that may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF
 * NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL
 * OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR
 * THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
 * OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 */

#include <SPI.h>

#define NCS_SPI_FLASH 10
#define NUM_DEVICES 18

#define println_hex(X) (Serial.println((X > 0x0F ? "0x" : "0x0") + String(X, HEX)))

struct device_profile
{
  uint8_t mfg_id_el;
  unsigned int dev_type_el;
  unsigned int dev_id_el;
  char teamName_el[15];
  char partName_el[15];
  unsigned char busyMask_el;
  unsigned long highest_addr_el;
};

const struct device_profile flash_device[NUM_DEVICES] = {
    //{ mfg_id, dev_typ, dev_id, teamName , partName,  busyMask, highest_addr },
    {0xBF, 0x25, 0x8C, "Microchip", "SST25VF020B", 0x01, 0x03FFFFF},
    {0xBF, 0x26, 0x02, "Microchip", "SST26VF032", 0x80, 0x03FFFFF},
    {0xBF, 0x25, 0x41, "Microchip", "SST25VF016B", 0x01, 0x01FFFFF},
    {0xBF, 0x25, 0x4B, "Microchip", "SST25VF064C", 0x01, 0x07FFFFF},
    {0xBF, 0x00, 0x00, "Microchip", "SST25VF032B", 0x01, 0x03FFFFF},
    {0xBF, 0x26, 0x43, "Microchip", "SST26VF064B", 0x01, 0x07FFFFF},
    {0xBF, 0x26, 0x42, "Microchip", "SST26VF032B", 0x01, 0x03FFFFF},
    {0xBF, 0x26, 0x51, "Microchip", "SST26WF016B", 0x01, 0x01FFFFF},
    {0xBF, 0x26, 0x41, "Microchip", "SST26VF016B", 0x01, 0x01FFFFF},
    {0xBF, 0x26, 0x58, "Microchip", "SST26WF080B", 0x01, 0x00FFFFF},
    {0xBF, 0x26, 0x54, "Microchip", "SST26WF040B", 0x01, 0x008FFFF},
    {0xC2, 0x20, 0x17, "Macronix", "MX25L6445E", 0x01, 0x07FFFFF},
    {0x20, 0x20, 0x16, "Micron", "M25P32-VMW6", 0x01, 0x03FFFFF},
    {0xEF, 0x40, 0x17, "Winbond", "W25Q64FVAAI", 0x01, 0x07FFFFF},
    {0x01, 0x40, 0x17, "Spansion", "S25FL164K", 0x01, 0x07FFFFF},
    {0x7F, 0x9D, 0x46, "ISSI", "IS25CQ032", 0x01, 0x03FFFFF},
    {0x1F, 0x46, 0x01, "Atmel", "AT26DF161A", 0x01, 0x01FFFFF},
    {0x00, 0x00, 0x00, "None", "Empty", 0x01, 0x01FFFFF}};

uint8_t fraction;
uint8_t mfg_id = 0;
uint8_t dev_type;
uint8_t dev_id;

unsigned char block_protection_10[18]; /* global array to store block_protection data */
const char *teamName, *partName;
uint8_t busyMask = 0;
uint32_t highestAddress;

void WriteEnable(void)
{
  digitalWrite(NCS_SPI_FLASH, 0);
  SPI.transfer(0x06);
  digitalWrite(NCS_SPI_FLASH, 1);
}

void ReadFlashID(void)
{
  digitalWrite(NCS_SPI_FLASH, 0);
  SPI.transfer(0x9F);
  mfg_id = SPI.transfer(0x00);
  dev_type = SPI.transfer(0x00);
  dev_id = SPI.transfer(0x00);
  Serial.println("Read ID:");
  Serial.print("  mfg_id: ");
  println_hex(mfg_id);
  Serial.print("dev_type: ");
  println_hex(dev_type);
  Serial.print("  dev_id: ");
  println_hex(dev_id);
  digitalWrite(NCS_SPI_FLASH, 1);
}

uint8_t ReadFlashStatus(void)
{
  unsigned int flashStatus = 0x00;
  digitalWrite(NCS_SPI_FLASH, 0);
  SPI.transfer(0x05);
  flashStatus = SPI.transfer(0x00);
  digitalWrite(NCS_SPI_FLASH, 1);
  return flashStatus;
}

void ClearBlockProtectSST1(void)
{
  uint8_t flashStatus1 = 0x00;

  // read status register first
  digitalWrite(NCS_SPI_FLASH, 0);
  SPI.transfer(0x05);
  flashStatus1 = SPI.transfer(0x00);
  digitalWrite(NCS_SPI_FLASH, 1);

  // modify flashstatus to clear protect bits
  flashStatus1 &= 0xC3;

  // write the new value to STATUS register
  digitalWrite(NCS_SPI_FLASH, 0);
  SPI.transfer(0x50);
  digitalWrite(NCS_SPI_FLASH, 1); // ENABLE WRITE STATUS REGISTER
  digitalWrite(NCS_SPI_FLASH, 0);
  SPI.transfer(0x01);
  SPI.transfer(flashStatus1);
  digitalWrite(NCS_SPI_FLASH, 0);
}

void ClearBlockProtect(void)
{
  unsigned char i = 0;
  if (dev_type == 0x25)
  {
    ClearBlockProtectSST1();
    return;
  }
  block_protection_10[0] = 0x00;
  block_protection_10[1] = 0x00;
  block_protection_10[2] = 0x00;
  block_protection_10[3] = 0x00;
  block_protection_10[4] = 0x00;
  block_protection_10[5] = 0x00;
  block_protection_10[6] = 0x00;
  block_protection_10[7] = 0x00;
  block_protection_10[8] = 0x00;
  block_protection_10[9] = 0x00;
  block_protection_10[10] = 0x00;
  block_protection_10[11] = 0x00;
  block_protection_10[12] = 0x00;
  block_protection_10[13] = 0x00;
  block_protection_10[14] = 0x00;
  block_protection_10[15] = 0x00;
  block_protection_10[16] = 0x00;
  block_protection_10[17] = 0x00;
  WriteEnable();
  digitalWrite(NCS_SPI_FLASH, 0);
  SPI.transfer(0x42);
  for (i = 18; i > 0; i--)
  {
    SPI.transfer(block_protection_10[i - 1]);
  }
  digitalWrite(NCS_SPI_FLASH, 1);

  // Florian's way for WINBOND
  if ((mfg_id == 0xEF) && (dev_type == 0x40) && (dev_id == 0x17))
  {
    WriteEnable();
    digitalWrite(NCS_SPI_FLASH, 0);
    SPI.transfer(0x01);
    SPI.transfer(0x00);
    digitalWrite(NCS_SPI_FLASH, 1);
    while (ReadFlashStatus() & busyMask)
      ;
  }
}

void ChipErase(void)
{
  ClearBlockProtect();
  WriteEnable();

  digitalWrite(NCS_SPI_FLASH, 0);
  SPI.transfer(0xC7);
  digitalWrite(NCS_SPI_FLASH, 1);
}

void setup()
{
  Serial.begin(115200);
  pinMode(NCS_SPI_FLASH, OUTPUT);
  SPI.begin();
}

void loop()
{
  ReadFlashID();

  unsigned char counter = 0;

  // println_hex(dev_id);
  while (busyMask == 0)
  {
    for (int i = 0; i < NUM_DEVICES; i++)
    {
      // println_hex(flash_device[i].dev_id_el);
      if ((flash_device[i].mfg_id_el == mfg_id) && (flash_device[i].dev_type_el == dev_type) && (flash_device[i].dev_id_el == dev_id))
      {
        Serial.println("--------------");
        Serial.println("Found it.");
        teamName = flash_device[i].teamName_el;
        partName = flash_device[i].partName_el;
        busyMask = flash_device[i].busyMask_el;
        Serial.println(teamName);
        Serial.println(partName);
        println_hex(busyMask);
        Serial.println("--------------");
      }
    }

    if (mfg_id == 0)
    {
      counter++;
      if (counter == 0)
      {
        Serial.println(partName);
        Serial.println("Please insert a supported device...");
      }
      delay(50);
    }
    else
    {
      Serial.println(partName);
      Serial.println("Device recognized!");
      delay(1000);
    }
    break;
  }

  unsigned long startTime = millis();
  unsigned long duration = 0;

  Serial.print("Erasing ");
  Serial.print(teamName);
  Serial.print("'s ");
  Serial.print(partName);
  Serial.println("...");

  ChipErase();

  while ((ReadFlashStatus() & busyMask) == 1)
  {
    duration = millis() - startTime;
    if ((duration > 0) && !(duration % 50))
    {
      Serial.print(".");
    }
    delay(1);
  }
  if (duration > 100)
  {
    Serial.println();
  }

  Serial.print("Done! ");
  Serial.print(teamName);
  Serial.print("'s ");
  Serial.print(partName);
  Serial.print(" took ");
  Serial.print(duration);
  Serial.println("ms to erase.");

  while (1)
  {
  }
}
