/*
 *
 * Copyright (c) 2014, weizhenwei
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the {organization} nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: recv_test.c 
 *
 * Brief: ortp receive main entry;
 *
 * Date: 2014.06.15
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "rtp_recv.h"

#if 0
const char * LOCAL_IP = "127.0.0.1";
const int LOCAL_PORT = 8000;

bool ortpClient()
{
	COrtpClient ortpClient;

	COrtpClient::init();	

	if (!ortpClient.create(LOCAL_IP_ADDR,LOCAL_RTP_PORT))
	{
		std::cout << "ortpClient.create fail!\n";
		getchar();
		getchar();
		return false;
	}

	char *buffer = new char[RECV_BUFFER_LEN];

	while(1)
	{
		int len = RECV_BUFFER_LEN;
		if (!ortpClient.get_recv_data(buffer,len))
		{
			Sleep(10);
			continue;
		}

		std::cout << "successful recv,data len =" << len << std::endl;
	}

	COrtpClient::deInit();

	delete [] buffer;

	return true;
}
#endif

void main()
{
    return 0;
}
