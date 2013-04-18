#pragma once

static class ByteUtil {
public:

	// ������� ����� ������ ��� ���������� ��������� ���������� �����
	static int GetByteLenForDataLen(int dataLen);

	// ���������� ��������� ����
	static byte GenerateByte();

	// ����������� ���� ����� � ���� ������� �� ������� ��������
	static byte FillDataInByte(byte data, int dataLen);

	// ������������� ���������� ������� ���� � �������� �������� �� ������� � hex
	static void ShowDataBlockOnConsole(byte *dataBlock, int dataBlockLen);
	static bool IsDataEqual(byte *dataFirst, byte *dataSecond, int dataLen);
	static byte *CopyData(byte *data, int len);
	static byte GetOnlyBitByte(byte &b, int bitPos);
	static byte InvertByte(byte b);
	static void SetBit(byte &b, int bitPos);
	static void UnsetBit(byte &b, int bitPos);
	static bool IsBitSettedInByte(byte &b, int bitPos);
};