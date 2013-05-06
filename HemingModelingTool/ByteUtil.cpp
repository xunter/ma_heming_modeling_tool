#include "StdAfx.h"
#include "ByteUtil.h"

// Считать число байтов для размещения заданного количество битов
int ByteUtil::GetByteLenForDataLen(int dataLen) {
	return ceil((float)dataLen / BYTE_BIT_LEN);	
};

// Генерирует случайный байт
byte ByteUtil::GenerateByte() {
	return (byte) (rand() % 26 + 'a');
};

// Упаковывает блок битов в байт начиная со старших разрядов
byte ByteUtil::FillDataInByte(byte data, int dataLen) {
	return data & ((byte)(0xff << (BYTE_BIT_LEN - dataLen)));
};

// Распечатывает содержимое массива байт с заданным размером на консоле в hex
void ByteUtil::ShowDataBlockOnConsole(byte *dataBlock, int dataBlockLen) {
	cout <<  hex << setfill('0');
	int bytesCount = GetByteLenForDataLen(dataBlockLen);
	for (int i = 0; i < bytesCount; i++) {
		cout << setw(2) << int(*(dataBlock + i)) << ' ';
	}
	cout << endl;
};

bool ByteUtil::IsDataEqual(byte *dataFirst, byte *dataSecond, int dataLen) {
	for (int i = 0; i < dataLen; i++) {
		if (dataFirst[i] != dataSecond[i]) return false;
	}
	return true;
};

byte *ByteUtil::CopyData(byte *data, int len) {
	byte *copyArr = new byte[len];
	for (int i = 0; i < len; i++) copyArr[i] = data[i];
	return copyArr;
};

byte ByteUtil::GetOnlyBitByte(byte &b, int bitPos) {		
	byte mask = (byte)pow(2.0, BYTE_BIT_LEN - bitPos - 1);
	return b & mask;
};

byte ByteUtil::InvertByte(byte b) {
	for (int i = 0; i < BYTE_BIT_LEN; i++) {
		byte bitByte = GetOnlyBitByte(b, i);
		if (!IsBitSettedInByte(b, i)) {
			SetBit(b, i);
		} else {
			UnsetBit(b, i);
		}
	}
	return b;
};

void ByteUtil::SetBit(byte &b, int bitPos) {
	byte ffByte = 0xff;
	b |= GetOnlyBitByte(ffByte, bitPos);
};

void ByteUtil::UnsetBit(byte &b, int bitPos) {
	byte ffByte = 0xff;
	b &= InvertByte(GetOnlyBitByte(ffByte, bitPos));
};

bool ByteUtil::IsBitSettedInByte(byte &b, int bitPos) {
	byte bitByte = GetOnlyBitByte(b, bitPos);
	return bitByte != 0x00;
};

void ByteUtil::QSort(byte *arr, int min, int max) {
	
	if(min >= max - 1)
        return;
    // Initially find a random pivot
    int pivotIndex = min + rand() % (max - min);
    int pivot = arr[pivotIndex];

	byte *begin = arr + min;
	byte *end = arr + (max - 1);

	// While begin != end 
    while(begin != end)
    {
        // Find the lowest bound number to swap
        while(*begin < pivot && begin < end)
            begin++;
        while(*end >= pivot && begin < end)
            end--;

            // Do the swap
        SwapBytes(begin, end);
    }

    // Here begin and end are equal and equal to point from where left side is lower and right side is greater or equal to pivot

    // Partition left
    QSort(arr, min, begin - arr);
    // Partiion right
    QSort(arr, end - arr, max);	
};

void ByteUtil::SwapBytes(byte *num, byte *num2)
{
    int temp = *num;
    *num = *num2;
    *num2 = temp;
};