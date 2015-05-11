#include "../stdafx.h"

template<typename T>
T get_value_type(ea_t ea)
{
	T val = (T)0;
	
	DbgMemRead((duint)ea, (unsigned char *)&val, sizeof(T));
	return val;
}

idaman ea_t ida_export nextaddr(ea_t ea)
{
	if (ea >= DB_GET_MAX_EA)
		return BADADDR;

	return ea + 1;
}

idaman ea_t ida_export prevaddr(ea_t ea)
{
	if (ea <= DB_GET_MIN_EA)
		return BADADDR;

	return ea - 1;
}

idaman uchar ida_export get_byte(ea_t ea)
{
	return get_value_type<uchar>(ea);
}

idaman uchar ida_export get_db_byte(ea_t ea)
{
	return get_byte(ea);
}

idaman ushort ida_export get_word(ea_t ea)
{
	return get_value_type<ushort>(ea);
}

idaman uint32 ida_export get_3byte(ea_t ea)
{
	ushort lower = get_word(ea) & 0xFFFF;
	ushort upper = get_byte(ea + sizeof(ushort)) & 0xFF;

	return (uint32)(lower + (upper << 16));
}

idaman uint32 ida_export get_long(ea_t ea)
{
	return get_value_type<uint32>(ea);
}

idaman uint64 ida_export get_qword(ea_t ea)
{
	return get_value_type<uint64>(ea);
}

idaman uint32 ida_export get_full_byte(ea_t ea)
{
	return get_byte(ea);
}

idaman uint64 ida_export get_full_word(ea_t ea)
{
	return get_word(ea);
}

idaman uint64 ida_export get_full_long(ea_t ea)
{
	return get_long(ea);
}

idaman uchar ida_export get_8bit(ea_t &ea, uint32 &v, int &nbit)
{
	__debugbreak();
	return 0;
}

uchar get_ascii_char(ea_t &ea, uint32 &v, int &nb)
{
	__debugbreak();
	return 0;
}

idaman uint32 ida_export get_16bit(ea_t ea)
{
	return get_word(ea);
}

idaman uint32 ida_export get_32bit(ea_t ea)
{
	return get_long(ea);
}

idaman uint64 ida_export get_64bit(ea_t ea)
{
	return get_qword(ea);
}

idaman bool ida_export get_many_bytes(ea_t ea, void *buf, ssize_t size)
{
	return DbgMemRead((duint)ea, (unsigned char *)buf, size);
}