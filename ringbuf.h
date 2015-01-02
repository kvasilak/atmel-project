/*************************************************************************
**   Ring buffer class
**
** Copyright 2010, 2015 Keith Vasilakes
**
** RingBuffer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation, either version 3 of the License, or (at your option) 
** any later version.
**
** This Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
** implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public 
** License for more details.
**
** You should have received a copy of the GNU General Public License along with MegaRide. If not, see
** http://www.gnu.org/licenses/.

**************************************************************************/

#ifndef RING_BUFFER_INCLUDED
#define RING_BUFFER_INCLUDED

enum ErrorT{
	ErrNone,
	ErrBufferFull,
	ErrBufferEmpty
} ;

//declaration and definition of the ringbuffer class
class CRingBuffer
{
public:
    CRingBuffer();
	void Init(uint8_t *const buffer, uint16_t size);
    uint16_t Len() const;
	uint16_t Free() const;
    uint16_t Size() const;
    void Clear();
    ErrorT Put(uint8_t c);
	void Add(uint8_t c);
	ErrorT Get(uint8_t *const c);
	uint8_t Read(uint16_t i);
	inline uint8_t operator[] (uint16_t i){return Read(i);}
		
	
private:
	bool IsOpen;
	uint16_t BufSize;
    uint8_t *Buffer;
    uint16_t In, Out;
	uint8_t cc;
};

#endif 
