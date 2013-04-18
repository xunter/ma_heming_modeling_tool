#pragma once

static class ByteUtil {
public:

	// Считать число байтов для размещения заданного количество битов
	static int GetByteLenForDataLen(int dataLen);

	// Генерирует случайный байт
	static byte GenerateByte();

	// Упаковывает блок битов в байт начиная со старших разрядов
	static byte FillDataInByte(byte data, int dataLen);

	// Распечатывает содержимое массива байт с заданным размером на консоле в hex
	static void ShowDataBlockOnConsole(byte *dataBlock, int dataBlockLen);
	static bool IsDataEqual(byte *dataFirst, byte *dataSecond, int dataLen);
	static byte *CopyData(byte *data, int len);
	static byte GetOnlyBitByte(byte &b, int bitPos);
	static byte InvertByte(byte b);
	static void SetBit(byte &b, int bitPos);
	static void UnsetBit(byte &b, int bitPos);
	static bool IsBitSettedInByte(byte &b, int bitPos);
};