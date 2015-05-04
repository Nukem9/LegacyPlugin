#pragma once

// Get next address in the program. (i.e. address which has flags)
// Return BADADDR if no such address exist.

idaman ea_t ida_export nextaddr(ea_t ea);


// Get previous address in the program.
// Return BADADDR if no such address exist.

idaman ea_t ida_export prevaddr(ea_t ea);

// Get one byte (8-bit) of the program at 'ea'
// This function works only for 8bit byte processors.

idaman uchar ida_export get_byte(ea_t ea);


// Get one byte (8-bit) of the program at 'ea' from the database
// even if the debugger is active.
// See also get_dbg_byte() to read the process memory directly.
// This function works only for 8bit byte processors.

idaman uchar ida_export get_db_byte(ea_t ea);


// Get one word (16-bit) of the program at 'ea'
// This function takes into account order of bytes specified in inf.mf
// This function works only for 8bit byte processors.

idaman ushort ida_export get_word(ea_t ea);


// Get one 3byte (24-bit) of the program at 'ea'
// This function takes into account order of bytes specified in inf.tribyte_order
// This function works only for 8bit byte processors.

idaman uint32 ida_export get_3byte(ea_t ea);


// Get one dword (32-bit) of the program at 'ea'
// This function takes into account order of bytes specified in inf.mf
// This function works only for 8bit byte processors.

idaman uint32 ida_export get_long(ea_t ea);


// Get one qword (64-bit) of the program at 'ea'
// This function takes into account order of bytes specified in inf.mf
// This function works only for 8bit byte processors.

idaman uint64 ida_export get_qword(ea_t ea);


// Get one wide byte of the program at 'ea'
// Some processors may access more than 8bit quantity at an address.
// These processors have 32-bit byte organization from the IDA's point of view.

idaman uint32 ida_export get_full_byte(ea_t ea);


// Get one wide word (2 'byte') of the program at 'ea'
// Some processors may access more than 8bit quantity at an address.
// These processors have 32-bit byte organization from the IDA's point of view.
// This function takes into account order of bytes specified in inf.mf

idaman uint64 ida_export get_full_word(ea_t ea);


// Get two wide words (4 'bytes') of the program at 'ea'
// Some processors may access more than 8bit quantity at an address.
// These processors have 32-bit byte organization from the IDA's point of view.
// This function takes into account order of bytes specified in inf.mf
// Note: this function works incorrectly if ph.nbits > 16

idaman uint64 ida_export get_full_long(ea_t ea);


// Get 8 bits of the program at 'ea'
// The main usage of this function is to iterate range of bytes.
// Here is an example:
//      uint32 v;
//      int nbit = 0;
//      for ( .... ) {
//        uchar byte = get_8bit(ea,v,nbit);
//        ...
//      }
// 'ea' is incremented each time when a new byte is read.
// In the above example, it will be incremented in the first loop iteration.

idaman uchar ida_export get_8bit(ea_t &ea, uint32 &v, int &nbit);
uchar get_ascii_char(ea_t &ea, uint32 &v, int &nb); // takes into account AS2_BYTE1CHAR


// Get 16bits of the program at 'ea'
// If the current processor has 16-bit byte, return 1 byte (getFullByte())
// Otherwise return get_word()

idaman uint32 ida_export get_16bit(ea_t ea);


// Get not more than 32bits of the program at 'ea'
// Depending on the ph.nbits, returns 32 bit value
//   if ( nbits <= 8 ) return get_long(ea);
//   if ( nbits <= 16) return get_full_word(ea);
//   return get_full_byte(ea);

idaman uint32 ida_export get_32bit(ea_t ea);


// Get not more than 64bits of the program at 'ea'
// Depending on the ph.nbits, returns 64 bit value
//   if ( nbits <= 8 ) return get_qword(ea);
//   if ( nbits <= 16) return get_full_long(ea);
//   return get_full_byte(ea);

idaman uint64 ida_export get_64bit(ea_t ea);

// Get the specified number of bytes of the program into the buffer.
//       ea - linear address
//       buf - buffer to hold bytes
//       size - size of buffer in normal 8-bit bytes (sizeof(buf))
// returns: 1-ok
//          0-failure
//            (out of program address space or uninited bytes, for example)

idaman bool ida_export get_many_bytes(ea_t ea, void *buf, ssize_t size);