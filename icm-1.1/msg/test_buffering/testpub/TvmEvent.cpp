
#include <algorithm>
#include <sstream>
#include "TvmEvent.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/Communicator.h>
#include <icc/Log.h>
#include <icc/Guard.h>

namespace TvmEvt1
{
  template<typename T>
  std::string __toString(T t)
  {
    std::ostringstream oss;
    oss << t;
    return oss.str();
  }
  
  std::string& trimEnd(std::string& str)
  {
    while(isspace(str[str.size() - 1]))
    {
      str = str.substr(0, str.size()-1);
    }
    return str;
  }
}

TvmEvt1::SanInfo::SanInfo()
{
  port = 0;
}

bool
TvmEvt1::SanInfo::operator==(const SanInfo& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::SanInfo::operator!=(const SanInfo& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(ip != __rhs.ip)
  {
    return true;
  }
  if(port != __rhs.port)
  {
    return true;
  }
  if(user != __rhs.user)
  {
    return true;
  }
  if(passwd != __rhs.passwd)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::SanInfo::__write(OutputStream* __os) const
{
  __os->write_string(ip);
  __os->write_short(port);
  __os->write_string(user);
  __os->write_string(passwd);
}

void
TvmEvt1::SanInfo::__read(InputStream* __is)
{
  __is->read_string(ip);
  __is->read_short(port);
  __is->read_string(user);
  __is->read_string(passwd);
}

std::string
TvmEvt1::SanInfo::toString() const
{
  std::string result("TvmEvt1::SanInfo-> ");
  result += ("ip:" + __toString(this->ip) + " ");
  result += ("port:" + __toString(this->port) + " ");
  result += ("user:" + __toString(this->user) + " ");
  result += ("passwd:" + __toString(this->passwd) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const SanInfo& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::CephInfo::CephInfo()
{
  port = 0;
}

bool
TvmEvt1::CephInfo::operator==(const CephInfo& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::CephInfo::operator!=(const CephInfo& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(ip != __rhs.ip)
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
TvmEvt1::CephInfo::__write(OutputStream* __os) const
{
  __os->write_string(ip);
  __os->write_short(port);
}

void
TvmEvt1::CephInfo::__read(InputStream* __is)
{
  __is->read_string(ip);
  __is->read_short(port);
}

std::string
TvmEvt1::CephInfo::toString() const
{
  std::string result("TvmEvt1::CephInfo-> ");
  result += ("ip:" + __toString(this->ip) + " ");
  result += ("port:" + __toString(this->port) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const CephInfo& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::VSwitchInfo::VSwitchInfo()
{
}

bool
TvmEvt1::VSwitchInfo::operator==(const VSwitchInfo& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::VSwitchInfo::operator!=(const VSwitchInfo& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(nicName != __rhs.nicName)
  {
    return true;
  }
  if(bridge != __rhs.bridge)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::VSwitchInfo::__write(OutputStream* __os) const
{
  __os->write_string(nicName);
  __os->write_string(bridge);
}

void
TvmEvt1::VSwitchInfo::__read(InputStream* __is)
{
  __is->read_string(nicName);
  __is->read_string(bridge);
}

std::string
TvmEvt1::VSwitchInfo::toString() const
{
  std::string result("TvmEvt1::VSwitchInfo-> ");
  result += ("nicName:" + __toString(this->nicName) + " ");
  result += ("bridge:" + __toString(this->bridge) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const VSwitchInfo& arg)
{
  return "[" + arg.toString() + "]";
}

std::string
TvmEvt1::__toString(const VSwitchs& arg)
{
  std::string result = "{";
  for(VSwitchs::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += __toString(*it);
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

void
TvmEvt1::__write(OutputStream* __os, const ::TvmEvt1::VSwitchInfo* begin, const ::TvmEvt1::VSwitchInfo* end, ::TvmEvt1::__U__VSwitchs)
{
  UInt size = static_cast< UInt>(end - begin);
  __os->write_uint(size);
  for(UInt i = 0; i < size; ++i)
  {
    begin[i].__write(__os);
  }
}

void
TvmEvt1::__read(InputStream* __is, ::TvmEvt1::VSwitchs& v, ::TvmEvt1::__U__VSwitchs)
{
  UInt sz;
  __is->read_uint(sz);
  v.resize(sz);
  for(UInt i = 0; i < sz; ++i)
  {
    v[i].__read(__is);
  }
}

TvmEvt1::StorageEngineInfo::StorageEngineInfo()
{
  port = 0;
}

bool
TvmEvt1::StorageEngineInfo::operator==(const StorageEngineInfo& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::StorageEngineInfo::operator!=(const StorageEngineInfo& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(ip != __rhs.ip)
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
TvmEvt1::StorageEngineInfo::__write(OutputStream* __os) const
{
  __os->write_string(ip);
  __os->write_short(port);
}

void
TvmEvt1::StorageEngineInfo::__read(InputStream* __is)
{
  __is->read_string(ip);
  __is->read_short(port);
}

std::string
TvmEvt1::StorageEngineInfo::toString() const
{
  std::string result("TvmEvt1::StorageEngineInfo-> ");
  result += ("ip:" + __toString(this->ip) + " ");
  result += ("port:" + __toString(this->port) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const StorageEngineInfo& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::Disk::Disk()
{
}

bool
TvmEvt1::Disk::operator==(const Disk& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::Disk::operator!=(const Disk& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(id != __rhs.id)
  {
    return true;
  }
  if(name != __rhs.name)
  {
    return true;
  }
  if(type != __rhs.type)
  {
    return true;
  }
  if(poolName != __rhs.poolName)
  {
    return true;
  }
  if(iqn != __rhs.iqn)
  {
    return true;
  }
  if(device != __rhs.device)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::Disk::__write(OutputStream* __os) const
{
  __os->write_string(id);
  __os->write_string(name);
  __os->write_string(type);
  __os->write_string(poolName);
  __os->write_string(iqn);
  __os->write_string(device);
}

void
TvmEvt1::Disk::__read(InputStream* __is)
{
  __is->read_string(id);
  __is->read_string(name);
  __is->read_string(type);
  __is->read_string(poolName);
  __is->read_string(iqn);
  __is->read_string(device);
}

std::string
TvmEvt1::Disk::toString() const
{
  std::string result("TvmEvt1::Disk-> ");
  result += ("id:" + __toString(this->id) + " ");
  result += ("name:" + __toString(this->name) + " ");
  result += ("type:" + __toString(this->type) + " ");
  result += ("poolName:" + __toString(this->poolName) + " ");
  result += ("iqn:" + __toString(this->iqn) + " ");
  result += ("device:" + __toString(this->device) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const Disk& arg)
{
  return "[" + arg.toString() + "]";
}

std::string
TvmEvt1::__toString(const Disks& arg)
{
  std::string result = "{";
  for(Disks::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += __toString(*it);
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

void
TvmEvt1::__write(OutputStream* __os, const ::TvmEvt1::Disk* begin, const ::TvmEvt1::Disk* end, ::TvmEvt1::__U__Disks)
{
  UInt size = static_cast< UInt>(end - begin);
  __os->write_uint(size);
  for(UInt i = 0; i < size; ++i)
  {
    begin[i].__write(__os);
  }
}

void
TvmEvt1::__read(InputStream* __is, ::TvmEvt1::Disks& v, ::TvmEvt1::__U__Disks)
{
  UInt sz;
  __is->read_uint(sz);
  v.resize(sz);
  for(UInt i = 0; i < sz; ++i)
  {
    v[i].__read(__is);
  }
}

TvmEvt1::RunInstanceRequest::RunInstanceRequest()
{
  seq = 0;
  cpu = 0;
  mem = 0;
}

bool
TvmEvt1::RunInstanceRequest::operator==(const RunInstanceRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::RunInstanceRequest::operator!=(const RunInstanceRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(cpu != __rhs.cpu)
  {
    return true;
  }
  if(mem != __rhs.mem)
  {
    return true;
  }
  if(imageId != __rhs.imageId)
  {
    return true;
  }
  if(networkType != __rhs.networkType)
  {
    return true;
  }
  if(vlanId != __rhs.vlanId)
  {
    return true;
  }
  if(netmask != __rhs.netmask)
  {
    return true;
  }
  if(gateway != __rhs.gateway)
  {
    return true;
  }
  if(ip != __rhs.ip)
  {
    return true;
  }
  if(mac != __rhs.mac)
  {
    return true;
  }
  if(bridge != __rhs.bridge)
  {
    return true;
  }
  if(hostNicName != __rhs.hostNicName)
  {
    return true;
  }
  if(port != __rhs.port)
  {
    return true;
  }
  if(diskList != __rhs.diskList)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::RunInstanceRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_int(cpu);
  __os->write_int(mem);
  __os->write_string(imageId);
  __os->write_string(networkType);
  __os->write_string(vlanId);
  __os->write_string(netmask);
  __os->write_string(gateway);
  __os->write_string(ip);
  __os->write_string(mac);
  __os->write_string(bridge);
  __os->write_string(hostNicName);
  __os->write_string(port);
  if(diskList.size() == 0)
  {
    __os->write_uint(0);
  }
  else
  {
    ::TvmEvt1::__write(__os, &diskList[0], &diskList[0] + diskList.size(), ::TvmEvt1::__U__Disks());
  }
}

void
TvmEvt1::RunInstanceRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_int(cpu);
  __is->read_int(mem);
  __is->read_string(imageId);
  __is->read_string(networkType);
  __is->read_string(vlanId);
  __is->read_string(netmask);
  __is->read_string(gateway);
  __is->read_string(ip);
  __is->read_string(mac);
  __is->read_string(bridge);
  __is->read_string(hostNicName);
  __is->read_string(port);
  ::TvmEvt1::__read(__is, diskList, ::TvmEvt1::__U__Disks());
}

std::string
TvmEvt1::RunInstanceRequest::toString() const
{
  std::string result("TvmEvt1::RunInstanceRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("cpu:" + __toString(this->cpu) + " ");
  result += ("mem:" + __toString(this->mem) + " ");
  result += ("imageId:" + __toString(this->imageId) + " ");
  result += ("networkType:" + __toString(this->networkType) + " ");
  result += ("vlanId:" + __toString(this->vlanId) + " ");
  result += ("netmask:" + __toString(this->netmask) + " ");
  result += ("gateway:" + __toString(this->gateway) + " ");
  result += ("ip:" + __toString(this->ip) + " ");
  result += ("mac:" + __toString(this->mac) + " ");
  result += ("bridge:" + __toString(this->bridge) + " ");
  result += ("hostNicName:" + __toString(this->hostNicName) + " ");
  result += ("port:" + __toString(this->port) + " ");
  result += ("diskList:" + __toString(this->diskList) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const RunInstanceRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::RunInstancesResult::RunInstancesResult()
{
  seq = 0;
  state = 0;
}

bool
TvmEvt1::RunInstancesResult::operator==(const RunInstancesResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::RunInstancesResult::operator!=(const RunInstancesResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  if(state != __rhs.state)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::RunInstancesResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_string(result);
  __os->write_int(state);
}

void
TvmEvt1::RunInstancesResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_string(result);
  __is->read_int(state);
}

std::string
TvmEvt1::RunInstancesResult::toString() const
{
  std::string result("TvmEvt1::RunInstancesResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("result:" + __toString(this->result) + " ");
  result += ("state:" + __toString(this->state) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const RunInstancesResult& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::RunInstanceProgress::RunInstanceProgress()
{
  state = 0;
  time = 0;
}

bool
TvmEvt1::RunInstanceProgress::operator==(const RunInstanceProgress& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::RunInstanceProgress::operator!=(const RunInstanceProgress& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(state != __rhs.state)
  {
    return true;
  }
  if(time != __rhs.time)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::RunInstanceProgress::__write(OutputStream* __os) const
{
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_int(state);
  __os->write_long(time);
}

void
TvmEvt1::RunInstanceProgress::__read(InputStream* __is)
{
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_int(state);
  __is->read_long(time);
}

std::string
TvmEvt1::RunInstanceProgress::toString() const
{
  std::string result("TvmEvt1::RunInstanceProgress-> ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("state:" + __toString(this->state) + " ");
  result += ("time:" + __toString(this->time) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const RunInstanceProgress& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::TerminateInstanceRequest::TerminateInstanceRequest()
{
  seq = 0;
}

bool
TvmEvt1::TerminateInstanceRequest::operator==(const TerminateInstanceRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::TerminateInstanceRequest::operator!=(const TerminateInstanceRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::TerminateInstanceRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
}

void
TvmEvt1::TerminateInstanceRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
}

std::string
TvmEvt1::TerminateInstanceRequest::toString() const
{
  std::string result("TvmEvt1::TerminateInstanceRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const TerminateInstanceRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::TerminateInstanceResult::TerminateInstanceResult()
{
  seq = 0;
  state = 0;
}

bool
TvmEvt1::TerminateInstanceResult::operator==(const TerminateInstanceResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::TerminateInstanceResult::operator!=(const TerminateInstanceResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  if(state != __rhs.state)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::TerminateInstanceResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_string(result);
  __os->write_int(state);
}

void
TvmEvt1::TerminateInstanceResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_string(result);
  __is->read_int(state);
}

std::string
TvmEvt1::TerminateInstanceResult::toString() const
{
  std::string result("TvmEvt1::TerminateInstanceResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("result:" + __toString(this->result) + " ");
  result += ("state:" + __toString(this->state) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const TerminateInstanceResult& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::StartInstanceRequest::StartInstanceRequest()
{
  seq = 0;
}

bool
TvmEvt1::StartInstanceRequest::operator==(const StartInstanceRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::StartInstanceRequest::operator!=(const StartInstanceRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::StartInstanceRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
}

void
TvmEvt1::StartInstanceRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
}

std::string
TvmEvt1::StartInstanceRequest::toString() const
{
  std::string result("TvmEvt1::StartInstanceRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const StartInstanceRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::StartInstanceResult::StartInstanceResult()
{
  seq = 0;
  state = 0;
}

bool
TvmEvt1::StartInstanceResult::operator==(const StartInstanceResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::StartInstanceResult::operator!=(const StartInstanceResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(state != __rhs.state)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::StartInstanceResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_int(state);
  __os->write_string(result);
}

void
TvmEvt1::StartInstanceResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_int(state);
  __is->read_string(result);
}

std::string
TvmEvt1::StartInstanceResult::toString() const
{
  std::string result("TvmEvt1::StartInstanceResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("state:" + __toString(this->state) + " ");
  result += ("result:" + __toString(this->result) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const StartInstanceResult& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::StopInstanceRequest::StopInstanceRequest()
{
  seq = 0;
}

bool
TvmEvt1::StopInstanceRequest::operator==(const StopInstanceRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::StopInstanceRequest::operator!=(const StopInstanceRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::StopInstanceRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
}

void
TvmEvt1::StopInstanceRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
}

std::string
TvmEvt1::StopInstanceRequest::toString() const
{
  std::string result("TvmEvt1::StopInstanceRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const StopInstanceRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::StopInstanceResult::StopInstanceResult()
{
  seq = 0;
  state = 0;
}

bool
TvmEvt1::StopInstanceResult::operator==(const StopInstanceResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::StopInstanceResult::operator!=(const StopInstanceResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(state != __rhs.state)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::StopInstanceResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_int(state);
  __os->write_string(result);
}

void
TvmEvt1::StopInstanceResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_int(state);
  __is->read_string(result);
}

std::string
TvmEvt1::StopInstanceResult::toString() const
{
  std::string result("TvmEvt1::StopInstanceResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("state:" + __toString(this->state) + " ");
  result += ("result:" + __toString(this->result) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const StopInstanceResult& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::RunScriptRequest::RunScriptRequest()
{
  seq = 0;
}

bool
TvmEvt1::RunScriptRequest::operator==(const RunScriptRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::RunScriptRequest::operator!=(const RunScriptRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(type != __rhs.type)
  {
    return true;
  }
  if(dir != __rhs.dir)
  {
    return true;
  }
  if(fname != __rhs.fname)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::RunScriptRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_string(type);
  __os->write_string(dir);
  __os->write_string(fname);
}

void
TvmEvt1::RunScriptRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_string(type);
  __is->read_string(dir);
  __is->read_string(fname);
}

std::string
TvmEvt1::RunScriptRequest::toString() const
{
  std::string result("TvmEvt1::RunScriptRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("type:" + __toString(this->type) + " ");
  result += ("dir:" + __toString(this->dir) + " ");
  result += ("fname:" + __toString(this->fname) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const RunScriptRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::RunScriptResult::RunScriptResult()
{
  seq = 0;
}

bool
TvmEvt1::RunScriptResult::operator==(const RunScriptResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::RunScriptResult::operator!=(const RunScriptResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::RunScriptResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_string(result);
}

void
TvmEvt1::RunScriptResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_string(result);
}

std::string
TvmEvt1::RunScriptResult::toString() const
{
  std::string result("TvmEvt1::RunScriptResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("result:" + __toString(this->result) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const RunScriptResult& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::QueryInstanceRequest::QueryInstanceRequest()
{
  seq = 0;
}

bool
TvmEvt1::QueryInstanceRequest::operator==(const QueryInstanceRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::QueryInstanceRequest::operator!=(const QueryInstanceRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::QueryInstanceRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
}

void
TvmEvt1::QueryInstanceRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
}

std::string
TvmEvt1::QueryInstanceRequest::toString() const
{
  std::string result("TvmEvt1::QueryInstanceRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const QueryInstanceRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::QueryInstanceResult::QueryInstanceResult()
{
  seq = 0;
  state = 0;
}

bool
TvmEvt1::QueryInstanceResult::operator==(const QueryInstanceResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::QueryInstanceResult::operator!=(const QueryInstanceResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(state != __rhs.state)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::QueryInstanceResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_int(state);
  __os->write_string(result);
}

void
TvmEvt1::QueryInstanceResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_int(state);
  __is->read_string(result);
}

std::string
TvmEvt1::QueryInstanceResult::toString() const
{
  std::string result("TvmEvt1::QueryInstanceResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("state:" + __toString(this->state) + " ");
  result += ("result:" + __toString(this->result) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const QueryInstanceResult& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::AttachVolumeRequest::AttachVolumeRequest()
{
  seq = 0;
}

bool
TvmEvt1::AttachVolumeRequest::operator==(const AttachVolumeRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::AttachVolumeRequest::operator!=(const AttachVolumeRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(volumeId != __rhs.volumeId)
  {
    return true;
  }
  if(iqn != __rhs.iqn)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(device != __rhs.device)
  {
    return true;
  }
  if(san != __rhs.san)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::AttachVolumeRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(volumeId);
  __os->write_string(iqn);
  __os->write_string(instanceId);
  __os->write_string(device);
  san.__write(__os);
}

void
TvmEvt1::AttachVolumeRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(volumeId);
  __is->read_string(iqn);
  __is->read_string(instanceId);
  __is->read_string(device);
  san.__read(__is);
}

std::string
TvmEvt1::AttachVolumeRequest::toString() const
{
  std::string result("TvmEvt1::AttachVolumeRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("volumeId:" + __toString(this->volumeId) + " ");
  result += ("iqn:" + __toString(this->iqn) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("device:" + __toString(this->device) + " ");
  result += ("san:" + __toString(this->san) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const AttachVolumeRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::AttachVolumeResult::AttachVolumeResult()
{
  seq = 0;
  attachTime = 0;
}

bool
TvmEvt1::AttachVolumeResult::operator==(const AttachVolumeResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::AttachVolumeResult::operator!=(const AttachVolumeResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(volumeId != __rhs.volumeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(device != __rhs.device)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  if(state != __rhs.state)
  {
    return true;
  }
  if(attachTime != __rhs.attachTime)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::AttachVolumeResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(volumeId);
  __os->write_string(instanceId);
  __os->write_string(device);
  __os->write_string(result);
  __os->write_string(state);
  __os->write_int(attachTime);
}

void
TvmEvt1::AttachVolumeResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(volumeId);
  __is->read_string(instanceId);
  __is->read_string(device);
  __is->read_string(result);
  __is->read_string(state);
  __is->read_int(attachTime);
}

std::string
TvmEvt1::AttachVolumeResult::toString() const
{
  std::string result("TvmEvt1::AttachVolumeResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("volumeId:" + __toString(this->volumeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("device:" + __toString(this->device) + " ");
  result += ("result:" + __toString(this->result) + " ");
  result += ("state:" + __toString(this->state) + " ");
  result += ("attachTime:" + __toString(this->attachTime) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const AttachVolumeResult& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::DetachVolumeRequest::DetachVolumeRequest()
{
  seq = 0;
}

bool
TvmEvt1::DetachVolumeRequest::operator==(const DetachVolumeRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::DetachVolumeRequest::operator!=(const DetachVolumeRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(volumeId != __rhs.volumeId)
  {
    return true;
  }
  if(iqn != __rhs.iqn)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(device != __rhs.device)
  {
    return true;
  }
  if(san != __rhs.san)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::DetachVolumeRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(volumeId);
  __os->write_string(iqn);
  __os->write_string(instanceId);
  __os->write_string(device);
  san.__write(__os);
}

void
TvmEvt1::DetachVolumeRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(volumeId);
  __is->read_string(iqn);
  __is->read_string(instanceId);
  __is->read_string(device);
  san.__read(__is);
}

std::string
TvmEvt1::DetachVolumeRequest::toString() const
{
  std::string result("TvmEvt1::DetachVolumeRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("volumeId:" + __toString(this->volumeId) + " ");
  result += ("iqn:" + __toString(this->iqn) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("device:" + __toString(this->device) + " ");
  result += ("san:" + __toString(this->san) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const DetachVolumeRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::DetachVolumeResult::DetachVolumeResult()
{
  seq = 0;
  detachTime = 0;
}

bool
TvmEvt1::DetachVolumeResult::operator==(const DetachVolumeResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::DetachVolumeResult::operator!=(const DetachVolumeResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(volumeId != __rhs.volumeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(device != __rhs.device)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  if(state != __rhs.state)
  {
    return true;
  }
  if(detachTime != __rhs.detachTime)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::DetachVolumeResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(volumeId);
  __os->write_string(instanceId);
  __os->write_string(device);
  __os->write_string(result);
  __os->write_string(state);
  __os->write_int(detachTime);
}

void
TvmEvt1::DetachVolumeResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(volumeId);
  __is->read_string(instanceId);
  __is->read_string(device);
  __is->read_string(result);
  __is->read_string(state);
  __is->read_int(detachTime);
}

std::string
TvmEvt1::DetachVolumeResult::toString() const
{
  std::string result("TvmEvt1::DetachVolumeResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("volumeId:" + __toString(this->volumeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("device:" + __toString(this->device) + " ");
  result += ("result:" + __toString(this->result) + " ");
  result += ("state:" + __toString(this->state) + " ");
  result += ("detachTime:" + __toString(this->detachTime) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const DetachVolumeResult& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::VmStat::VmStat()
{
  memory = 0;
  cpu = 0;
  rxBytes = 0;
  rxPackets = 0;
  txBytes = 0;
  txPackets = 0;
}

bool
TvmEvt1::VmStat::operator==(const VmStat& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::VmStat::operator!=(const VmStat& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(memory != __rhs.memory)
  {
    return true;
  }
  if(cpu != __rhs.cpu)
  {
    return true;
  }
  if(rxBytes != __rhs.rxBytes)
  {
    return true;
  }
  if(rxPackets != __rhs.rxPackets)
  {
    return true;
  }
  if(txBytes != __rhs.txBytes)
  {
    return true;
  }
  if(txPackets != __rhs.txPackets)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::VmStat::__write(OutputStream* __os) const
{
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_int(memory);
  __os->write_int(cpu);
  __os->write_long(rxBytes);
  __os->write_long(rxPackets);
  __os->write_long(txBytes);
  __os->write_long(txPackets);
}

void
TvmEvt1::VmStat::__read(InputStream* __is)
{
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_int(memory);
  __is->read_int(cpu);
  __is->read_long(rxBytes);
  __is->read_long(rxPackets);
  __is->read_long(txBytes);
  __is->read_long(txPackets);
}

std::string
TvmEvt1::VmStat::toString() const
{
  std::string result("TvmEvt1::VmStat-> ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("instanceId:" + __toString(this->instanceId) + " ");
  result += ("memory:" + __toString(this->memory) + " ");
  result += ("cpu:" + __toString(this->cpu) + " ");
  result += ("rxBytes:" + __toString(this->rxBytes) + " ");
  result += ("rxPackets:" + __toString(this->rxPackets) + " ");
  result += ("txBytes:" + __toString(this->txBytes) + " ");
  result += ("txPackets:" + __toString(this->txPackets) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const VmStat& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::RegisterNodeRequest::RegisterNodeRequest()
{
  seq = 0;
  totalCpu = 0;
  totalMem = 0;
  totalDisk = 0;
}

bool
TvmEvt1::RegisterNodeRequest::operator==(const RegisterNodeRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::RegisterNodeRequest::operator!=(const RegisterNodeRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(networkType != __rhs.networkType)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(hostname != __rhs.hostname)
  {
    return true;
  }
  if(nodeIp != __rhs.nodeIp)
  {
    return true;
  }
  if(nodeOs != __rhs.nodeOs)
  {
    return true;
  }
  if(initiatorIqn != __rhs.initiatorIqn)
  {
    return true;
  }
  if(totalCpu != __rhs.totalCpu)
  {
    return true;
  }
  if(totalMem != __rhs.totalMem)
  {
    return true;
  }
  if(totalDisk != __rhs.totalDisk)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::RegisterNodeRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(networkType);
  __os->write_string(nodeId);
  __os->write_string(hostname);
  __os->write_string(nodeIp);
  __os->write_string(nodeOs);
  __os->write_string(initiatorIqn);
  __os->write_int(totalCpu);
  __os->write_long(totalMem);
  __os->write_long(totalDisk);
}

void
TvmEvt1::RegisterNodeRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(networkType);
  __is->read_string(nodeId);
  __is->read_string(hostname);
  __is->read_string(nodeIp);
  __is->read_string(nodeOs);
  __is->read_string(initiatorIqn);
  __is->read_int(totalCpu);
  __is->read_long(totalMem);
  __is->read_long(totalDisk);
}

std::string
TvmEvt1::RegisterNodeRequest::toString() const
{
  std::string result("TvmEvt1::RegisterNodeRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("networkType:" + __toString(this->networkType) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("hostname:" + __toString(this->hostname) + " ");
  result += ("nodeIp:" + __toString(this->nodeIp) + " ");
  result += ("nodeOs:" + __toString(this->nodeOs) + " ");
  result += ("initiatorIqn:" + __toString(this->initiatorIqn) + " ");
  result += ("totalCpu:" + __toString(this->totalCpu) + " ");
  result += ("totalMem:" + __toString(this->totalMem) + " ");
  result += ("totalDisk:" + __toString(this->totalDisk) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const RegisterNodeRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::RegisterNodeResult::RegisterNodeResult()
{
  seq = 0;
}

bool
TvmEvt1::RegisterNodeResult::operator==(const RegisterNodeResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::RegisterNodeResult::operator!=(const RegisterNodeResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(san != __rhs.san)
  {
    return true;
  }
  if(ceph != __rhs.ceph)
  {
    return true;
  }
  if(storageEninge != __rhs.storageEninge)
  {
    return true;
  }
  if(vswitchList != __rhs.vswitchList)
  {
    return true;
  }
  if(bootImgLoc != __rhs.bootImgLoc)
  {
    return true;
  }
  if(bootImgPoolName != __rhs.bootImgPoolName)
  {
    return true;
  }
  if(addrAssignMode != __rhs.addrAssignMode)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::RegisterNodeResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  san.__write(__os);
  ceph.__write(__os);
  storageEninge.__write(__os);
  if(vswitchList.size() == 0)
  {
    __os->write_uint(0);
  }
  else
  {
    ::TvmEvt1::__write(__os, &vswitchList[0], &vswitchList[0] + vswitchList.size(), ::TvmEvt1::__U__VSwitchs());
  }
  __os->write_string(bootImgLoc);
  __os->write_string(bootImgPoolName);
  __os->write_string(addrAssignMode);
  __os->write_string(result);
}

void
TvmEvt1::RegisterNodeResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  san.__read(__is);
  ceph.__read(__is);
  storageEninge.__read(__is);
  ::TvmEvt1::__read(__is, vswitchList, ::TvmEvt1::__U__VSwitchs());
  __is->read_string(bootImgLoc);
  __is->read_string(bootImgPoolName);
  __is->read_string(addrAssignMode);
  __is->read_string(result);
}

std::string
TvmEvt1::RegisterNodeResult::toString() const
{
  std::string result("TvmEvt1::RegisterNodeResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("san:" + __toString(this->san) + " ");
  result += ("ceph:" + __toString(this->ceph) + " ");
  result += ("storageEninge:" + __toString(this->storageEninge) + " ");
  result += ("vswitchList:" + __toString(this->vswitchList) + " ");
  result += ("bootImgLoc:" + __toString(this->bootImgLoc) + " ");
  result += ("bootImgPoolName:" + __toString(this->bootImgPoolName) + " ");
  result += ("addrAssignMode:" + __toString(this->addrAssignMode) + " ");
  result += ("result:" + __toString(this->result) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const RegisterNodeResult& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::UnRegisterNodeRequest::UnRegisterNodeRequest()
{
  seq = 0;
}

bool
TvmEvt1::UnRegisterNodeRequest::operator==(const UnRegisterNodeRequest& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::UnRegisterNodeRequest::operator!=(const UnRegisterNodeRequest& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::UnRegisterNodeRequest::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
}

void
TvmEvt1::UnRegisterNodeRequest::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
}

std::string
TvmEvt1::UnRegisterNodeRequest::toString() const
{
  std::string result("TvmEvt1::UnRegisterNodeRequest-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const UnRegisterNodeRequest& arg)
{
  return "[" + arg.toString() + "]";
}

TvmEvt1::UnRegisterNodeResult::UnRegisterNodeResult()
{
  seq = 0;
}

bool
TvmEvt1::UnRegisterNodeResult::operator==(const UnRegisterNodeResult& __rhs) const
{
  return !operator!=(__rhs);
}

bool
TvmEvt1::UnRegisterNodeResult::operator!=(const UnRegisterNodeResult& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  return false;
}

void
TvmEvt1::UnRegisterNodeResult::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(result);
}

void
TvmEvt1::UnRegisterNodeResult::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(result);
}

std::string
TvmEvt1::UnRegisterNodeResult::toString() const
{
  std::string result("TvmEvt1::UnRegisterNodeResult-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("nodeId:" + __toString(this->nodeId) + " ");
  result += ("result:" + __toString(this->result) + " ");
  return trimEnd(result);
}

std::string
TvmEvt1::__toString(const UnRegisterNodeResult& arg)
{
  return "[" + arg.toString() + "]";
}
