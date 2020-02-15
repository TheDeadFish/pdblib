#pragma once

struct MsfFile : 
	xArray<xArray<byte>>
{
	int load(cch* file);
};
