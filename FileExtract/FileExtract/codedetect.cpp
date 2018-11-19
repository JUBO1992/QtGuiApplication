#include "codedetect.h"

CodeDetect::CodeDetect()
{

}

CodeDetect::~CodeDetect()
{

}

EncodingType CodeDetect::GetEncodingType(QString filepath)
{
	QFile file(filepath);
	return GetEncodingType(file);
}

EncodingType CodeDetect::GetEncodingType(QFile &file)
{
	uchar* src = file.map(0, file.size());
	EncodingType type = GetEncodingTypeFromStr(src, file.size());
	return type;
}

BOOL CodeDetect::CheckUTF8NoBOM(const void* pBuffer, long size)
{
	bool IsUTF8 = true;
	unsigned char* start = (unsigned char*)pBuffer;
	unsigned char* end = (unsigned char*)pBuffer + size;
	while (start < end)
	{
		if (*start < 0x80) {
			// (10000000): 值小于0x80的为ASCII字符  
			start++;
		}
		else if (*start < (0xC0)) {
			// (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符
			IsUTF8 = false;
			break;
		}
		else if (*start < (0xE0)) {
			// (11100000): 此范围内为2字节UTF-8字符
			if (start >= end - 1) break;
			if ((start[1] & (0xC0)) != 0x80) {
				IsUTF8 = false;
				break;
			}
			start += 2;
		}
		else if (*start < (0xF0)) {
			// (11110000): 此范围内为3字节UTF-8字符
			if (start >= end - 2) break;
			if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80) {
				IsUTF8 = false;
				break;
			}
			start += 3;
		}
		else {
			IsUTF8 = false;
			break;
		}
	}
	return IsUTF8;
}

EncodingType CodeDetect::GetEncodingTypeFromStr(const void* src, long len)
{
	const PBYTE pBuffer = (const PBYTE)src;
	if (pBuffer == NULL || len < 3)
		return ENCODINGTYPE_UNKNOW;
	if (pBuffer[0] == 0xFF && pBuffer[1] == 0xFE)
		return ENCODINGTYPE_ULE;
	if (pBuffer[0] == 0xFE && pBuffer[1] == 0xFF)
		return ENCODINGTYPE_UBE;
	if (pBuffer[0] == 0xEF && pBuffer[1] == 0xBB && pBuffer[2] == 0xBF)
		return ENCODINGTYPE_UTF8;
	if (CheckUTF8NoBOM(src, len))
		return ENCODINGTYPE_UTF8;
	else return ENCODINGTYPE_ANSI;
}

wstring CodeDetect::StrToWstr(UINT CodePage, const string& str)
{
	int len = str.length();
	wstring  wStr = L"";
	if (len <= 0) return wStr;

	int  unicodeLen = ::MultiByteToWideChar(CodePage, 0, str.c_str(), -1, NULL, 0);

	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CodePage, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);

	wStr = (wchar_t*)pUnicode;
	delete  pUnicode;
	return  wStr;
}

string CodeDetect::WstrToStr(UINT CodePage, const wstring& wStr)
{
	int len = wStr.length();
	string  str = "";
	if (len <= 0) return str;

	char*     pElementText;
	int    iTextLen;

	iTextLen = WideCharToMultiByte(CodePage, 0, wStr.c_str(), -1, NULL, 0, NULL, NULL);

	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
	::WideCharToMultiByte(CodePage, 0, wStr.c_str(), -1, pElementText, iTextLen, NULL, NULL);

	str = pElementText;
	delete[] pElementText;

	return str;
}

wstring CodeDetect::ANSIToUnicode(const string& strANSI)
{
	return StrToWstr(CP_ACP, strANSI);
}

wstring CodeDetect::UTF8ToUnicode(const string& strUTF8)
{
	return StrToWstr(CP_UTF8, strUTF8);
}

string CodeDetect::UnicodeToANSI(const wstring& strUnicode)
{
	return  WstrToStr(CP_ACP, strUnicode);
}

string CodeDetect::UnicodeToUTF8(const wstring& strUnicode)
{
	return  WstrToStr(CP_UTF8, strUnicode);
}
