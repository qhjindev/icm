
#include <algorithm>
#include <string>
#include "CommonService.h"
#include <icm/ServerRequest.h>
#include <icc/CdrStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>

bool
KFS::FileAttr::operator==(const FileAttr& __rhs) const
{
    return !operator!=(__rhs);
}

bool
KFS::FileAttr::operator!=(const FileAttr& __rhs) const
{
    if(this == &__rhs)
    {
	return false;
    }
    if(fileid != __rhs.fileid)
    {
	return true;
    }
    if(mtime != __rhs.mtime)
    {
	return true;
    }
    if(ctime != __rhs.ctime)
    {
	return true;
    }
    if(crtime != __rhs.crtime)
    {
	return true;
    }
    if(chunkCount != __rhs.chunkCount)
    {
	return true;
    }
    if(fileSize != __rhs.fileSize)
    {
	return true;
    }
    if(type != __rhs.type)
    {
	return true;
    }
    if(numReplicas != __rhs.numReplicas)
    {
	return true;
    }
    return false;
}

void
KFS::FileAttr::__write(OutputStream* __os) const
{
    __os.write_long(fileid);
    __os.write_long(mtime);
    __os.write_long(ctime);
    __os.write_long(crtime);
    __os.write_long(chunkCount);
    __os.write_long(fileSize);
    __os.write_string(type);
    __os.write_short(numReplicas);
}

void
KFS::FileAttr::__read(InputStream* __is)
{
    __is.read_long(fileid);
    __is.read_long(mtime);
    __is.read_long(ctime);
    __is.read_long(crtime);
    __is.read_long(chunkCount);
    __is.read_long(fileSize);
    __is.read_string(type);
    __is.read_short(numReplicas);
}

bool
KFS::SvrLoc::operator==(const SvrLoc& __rhs) const
{
    return !operator!=(__rhs);
}

bool
KFS::SvrLoc::operator!=(const SvrLoc& __rhs) const
{
    if(this == &__rhs)
    {
	return false;
    }
    if(hostname != __rhs.hostname)
    {
	return true;
    }
    if(port != __rhs.port)
    {
	return true;
    }
    return false;
}

void
KFS::SvrLoc::__write(OutputStream* __os) const
{
    __os.write_string(hostname);
    __os.write_short(port);
}

void
KFS::SvrLoc::__read(InputStream* __is)
{
    __is.read_string(hostname);
    __is.read_short(port);
}

void
KFS::__write(OutputStream* __os, const ::KFS::SvrLoc* begin, const ::KFS::SvrLoc* end, ::KFS::__U__SvrLocSeq)
{
    Short size = static_cast< Short>(end - begin);
    __os->write_short(size);
    for(int i = 0; i < size; ++i)
    {
	begin[i].__write(__os);
    }
}

void
KFS::__read(InputStream* __is, ::KFS::SvrLocSeq& v, ::KFS::__U__SvrLocSeq)
{
    Short sz;
    __is->read_short(sz);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
	v[i].__read(__is);
    }
}

bool
KFS::ChunkLayoutInfo::operator==(const ChunkLayoutInfo& __rhs) const
{
    return !operator!=(__rhs);
}

bool
KFS::ChunkLayoutInfo::operator!=(const ChunkLayoutInfo& __rhs) const
{
    if(this == &__rhs)
    {
	return false;
    }
    if(fileOffset != __rhs.fileOffset)
    {
	return true;
    }
    if(chunkId != __rhs.chunkId)
    {
	return true;
    }
    if(chunkVersion != __rhs.chunkVersion)
    {
	return true;
    }
    if(chunkServers != __rhs.chunkServers)
    {
	return true;
    }
    return false;
}

void
KFS::ChunkLayoutInfo::__write(OutputStream* __os) const
{
    __os.write_long(fileOffset);
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    if(chunkServers.size() == 0)
    {
	__os.write_short(0);
    }
    else
    {
	::KFS::__write(__os, &chunkServers[0], &chunkServers[0] + chunkServers.size(), ::KFS::__U__SvrLocSeq());
    }
}

void
KFS::ChunkLayoutInfo::__read(InputStream* __is)
{
    __is.read_long(fileOffset);
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    ::KFS::__read(__is, chunkServers, ::KFS::__U__SvrLocSeq());
}

void
KFS::__write(OutputStream* __os, const ::KFS::ChunkLayoutInfo* begin, const ::KFS::ChunkLayoutInfo* end, ::KFS::__U__ChunkLayoutInfoSeq)
{
    Short size = static_cast< Short>(end - begin);
    __os->write_short(size);
    for(int i = 0; i < size; ++i)
    {
	begin[i].__write(__os);
    }
}

void
KFS::__read(InputStream* __is, ::KFS::ChunkLayoutInfoSeq& v, ::KFS::__U__ChunkLayoutInfoSeq)
{
    Short sz;
    __is->read_short(sz);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
	v[i].__read(__is);
    }
}

bool
KFS::ChunkInfo::operator==(const ChunkInfo& __rhs) const
{
    return !operator!=(__rhs);
}

bool
KFS::ChunkInfo::operator!=(const ChunkInfo& __rhs) const
{
    if(this == &__rhs)
    {
	return false;
    }
    if(fileId != __rhs.fileId)
    {
	return true;
    }
    if(chunkId != __rhs.chunkId)
    {
	return true;
    }
    if(chunkVersion != __rhs.chunkVersion)
    {
	return true;
    }
    return false;
}

void
KFS::ChunkInfo::__write(OutputStream* __os) const
{
    __os.write_long(fileId);
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
}

void
KFS::ChunkInfo::__read(InputStream* __is)
{
    __is.read_long(fileId);
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
}

void
KFS::__write(OutputStream* __os, const ::KFS::ChunkInfo* begin, const ::KFS::ChunkInfo* end, ::KFS::__U__ChunkInfoSeq)
{
    Short size = static_cast< Short>(end - begin);
    __os->write_short(size);
    for(int i = 0; i < size; ++i)
    {
	begin[i].__write(__os);
    }
}

void
KFS::__read(InputStream* __is, ::KFS::ChunkInfoSeq& v, ::KFS::__U__ChunkInfoSeq)
{
    Short sz;
    __is->read_short(sz);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
	v[i].__read(__is);
    }
}

bool
KFS::WriteInfo::operator==(const WriteInfo& __rhs) const
{
    return !operator!=(__rhs);
}

bool
KFS::WriteInfo::operator!=(const WriteInfo& __rhs) const
{
    if(this == &__rhs)
    {
	return false;
    }
    if(serverLoc != __rhs.serverLoc)
    {
	return true;
    }
    if(writeId != __rhs.writeId)
    {
	return true;
    }
    return false;
}

void
KFS::WriteInfo::__write(OutputStream* __os) const
{
    serverLoc.__write(__os);
    __os.write_long(writeId);
}

void
KFS::WriteInfo::__read(InputStream* __is)
{
    serverLoc.__read(__is);
    __is.read_long(writeId);
}

void
KFS::__write(OutputStream* __os, const ::KFS::WriteInfo* begin, const ::KFS::WriteInfo* end, ::KFS::__U__WriteInfoSeq)
{
    Short size = static_cast< Short>(end - begin);
    __os->write_short(size);
    for(int i = 0; i < size; ++i)
    {
	begin[i].__write(__os);
    }
}

void
KFS::__read(InputStream* __is, ::KFS::WriteInfoSeq& v, ::KFS::__U__WriteInfoSeq)
{
    Short sz;
    __is->read_short(sz);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
	v[i].__read(__is);
    }
}
