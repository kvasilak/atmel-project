/*************************************************************************
**   RingBuffer
**
** Copyright 2010 Keith Vasilakes
**
** This software is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation, either version 3 of the License, or (at your option) 
** any later version.
**
** This software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
** implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public 
** License for more details.
**
** You should have received a copy of the GNU General Public License along with MegaRide. If not, see
** http://www.gnu.org/licenses/.

**************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "ringbuf.h"

CRingBuffer::CRingBuffer()
{
	
}

void CRingBuffer::Init(uint8_t *const buf, uint16_t size)
{
	IsOpen = false;
    In = Out = 0;
	
	if(size != 0)
	{
		BufSize = size;
		
		Buffer = (uint8_t*)buf;
		IsOpen = true;
	}
}

//get the available bytes left in the buffer
uint16_t CRingBuffer::Free() const
{
	return BufSize - Len();
}

//Get the size of the buffer in bytes
uint16_t CRingBuffer::Size() const
{
    return BufSize;
}

//Get the number of bytes in the buffer
uint16_t CRingBuffer::Len() const
{
	if(Out > In)
	{
		return BufSize - Out + In;
	}
	else
	{
		return  Out - In;
	}
}

//Remove all data from the buffer
void CRingBuffer::Clear()
{
    In = 0;
	Out = 0;
}

//Put a Byte into the buffer
ErrorT CRingBuffer::Put(uint8_t c)
{
	uint16_t in;

	in = In +1;

	//handle wrap
	if( in >= BufSize-1)
	{
	  in = 0;
	}
			
  //buffer is full?
  if(in != Out)
  {
    //All good, give Mr Creosote a wafer
    Buffer[In] = c;
    
    In = in;
      
    return ErrNone;
  }
  
  return ErrBufferFull;
}

//allows adding after buffer is full 
void CRingBuffer::Add(uint8_t c)
{
    Buffer[In++] = c;

	//handle wrap
	if( In >= BufSize)
	  In = 0;
}

//Get a byte from the buffer
//Removes byte
ErrorT CRingBuffer::Get(uint8_t *const c)
{
//  assert(c == NULL);
  
  //buffer is empty?
  if(In != Out)
  {
    //not empty yet
    *c = Buffer[Out];
    
    Out++;
    
    //handle wrap
    if(Out > BufSize-1)
    {
      Out = 0; 
    }
    
    return ErrNone;
  }
    
  return ErrBufferEmpty;
}

//Read the value at the index, do not remove from buffer
uint8_t CRingBuffer::Read(uint16_t i)
{
	uint8_t c=0;
	//index is within buffer?
	if(i < BufSize)
	{
		//not empty yet
		c = Buffer[i];
	}
	return c;
}

