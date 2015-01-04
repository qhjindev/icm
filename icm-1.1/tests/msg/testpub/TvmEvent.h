
#ifndef __TvmEvent_h__
#define __TvmEvent_h__

#include <string>
#include <vector>
#include <list>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>
class ServerRequest;

namespace TvmEvt1
{

struct SanInfo
{
  SanInfo();
  ::std::string ip;
  Short port;
  ::std::string user;
  ::std::string passwd;

  bool operator==(const SanInfo&) const;
  bool operator!=(const SanInfo&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const SanInfo&);

struct CephInfo
{
  CephInfo();
  ::std::string ip;
  Short port;

  bool operator==(const CephInfo&) const;
  bool operator!=(const CephInfo&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const CephInfo&);

struct VSwitchInfo
{
  VSwitchInfo();
  ::std::string nicName;
  ::std::string bridge;

  bool operator==(const VSwitchInfo&) const;
  bool operator!=(const VSwitchInfo&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const VSwitchInfo&);

typedef ::std::vector< ::TvmEvt1::VSwitchInfo> VSwitchs;

class __U__VSwitchs { };
std::string __toString(const VSwitchs&);
void __write(OutputStream*, const ::TvmEvt1::VSwitchInfo*, const ::TvmEvt1::VSwitchInfo*, __U__VSwitchs);
void __read(InputStream*, VSwitchs&, __U__VSwitchs);

struct StorageEngineInfo
{
  StorageEngineInfo();
  ::std::string ip;
  Short port;

  bool operator==(const StorageEngineInfo&) const;
  bool operator!=(const StorageEngineInfo&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const StorageEngineInfo&);

struct Disk
{
  Disk();
  ::std::string id;
  ::std::string name;
  ::std::string type;
  ::std::string poolName;
  ::std::string iqn;
  ::std::string device;

  bool operator==(const Disk&) const;
  bool operator!=(const Disk&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const Disk&);

typedef ::std::vector< ::TvmEvt1::Disk> Disks;

class __U__Disks { };
std::string __toString(const Disks&);
void __write(OutputStream*, const ::TvmEvt1::Disk*, const ::TvmEvt1::Disk*, __U__Disks);
void __read(InputStream*, Disks&, __U__Disks);

struct RunInstanceRequest
{
  RunInstanceRequest();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  Int cpu;
  Int mem;
  ::std::string imageId;
  ::std::string networkType;
  ::std::string vlanId;
  ::std::string netmask;
  ::std::string gateway;
  ::std::string ip;
  ::std::string mac;
  ::std::string bridge;
  ::std::string hostNicName;
  ::std::string port;
  ::TvmEvt1::Disks diskList;

  bool operator==(const RunInstanceRequest&) const;
  bool operator!=(const RunInstanceRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const RunInstanceRequest&);

struct RunInstancesResult
{
  RunInstancesResult();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  ::std::string result;
  Int state;

  bool operator==(const RunInstancesResult&) const;
  bool operator!=(const RunInstancesResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const RunInstancesResult&);

struct RunInstanceProgress
{
  RunInstanceProgress();
  ::std::string nodeId;
  ::std::string instanceId;
  Int state;
  Long time;

  bool operator==(const RunInstanceProgress&) const;
  bool operator!=(const RunInstanceProgress&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const RunInstanceProgress&);

struct TerminateInstanceRequest
{
  TerminateInstanceRequest();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;

  bool operator==(const TerminateInstanceRequest&) const;
  bool operator!=(const TerminateInstanceRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const TerminateInstanceRequest&);

struct TerminateInstanceResult
{
  TerminateInstanceResult();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  ::std::string result;
  Int state;

  bool operator==(const TerminateInstanceResult&) const;
  bool operator!=(const TerminateInstanceResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const TerminateInstanceResult&);

struct StartInstanceRequest
{
  StartInstanceRequest();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;

  bool operator==(const StartInstanceRequest&) const;
  bool operator!=(const StartInstanceRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const StartInstanceRequest&);

struct StartInstanceResult
{
  StartInstanceResult();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  Int state;
  ::std::string result;

  bool operator==(const StartInstanceResult&) const;
  bool operator!=(const StartInstanceResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const StartInstanceResult&);

struct StopInstanceRequest
{
  StopInstanceRequest();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;

  bool operator==(const StopInstanceRequest&) const;
  bool operator!=(const StopInstanceRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const StopInstanceRequest&);

struct StopInstanceResult
{
  StopInstanceResult();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  Int state;
  ::std::string result;

  bool operator==(const StopInstanceResult&) const;
  bool operator!=(const StopInstanceResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const StopInstanceResult&);

struct RunScriptRequest
{
  RunScriptRequest();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  ::std::string type;
  ::std::string dir;
  ::std::string fname;

  bool operator==(const RunScriptRequest&) const;
  bool operator!=(const RunScriptRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const RunScriptRequest&);

struct RunScriptResult
{
  RunScriptResult();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  ::std::string result;

  bool operator==(const RunScriptResult&) const;
  bool operator!=(const RunScriptResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const RunScriptResult&);

struct QueryInstanceRequest
{
  QueryInstanceRequest();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;

  bool operator==(const QueryInstanceRequest&) const;
  bool operator!=(const QueryInstanceRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const QueryInstanceRequest&);

struct QueryInstanceResult
{
  QueryInstanceResult();
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  Int state;
  ::std::string result;

  bool operator==(const QueryInstanceResult&) const;
  bool operator!=(const QueryInstanceResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const QueryInstanceResult&);

struct AttachVolumeRequest
{
  AttachVolumeRequest();
  Int seq;
  ::std::string nodeId;
  ::std::string volumeId;
  ::std::string iqn;
  ::std::string instanceId;
  ::std::string device;
  ::TvmEvt1::SanInfo san;

  bool operator==(const AttachVolumeRequest&) const;
  bool operator!=(const AttachVolumeRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const AttachVolumeRequest&);

struct AttachVolumeResult
{
  AttachVolumeResult();
  Int seq;
  ::std::string nodeId;
  ::std::string volumeId;
  ::std::string instanceId;
  ::std::string device;
  ::std::string result;
  ::std::string state;
  Int attachTime;

  bool operator==(const AttachVolumeResult&) const;
  bool operator!=(const AttachVolumeResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const AttachVolumeResult&);

struct DetachVolumeRequest
{
  DetachVolumeRequest();
  Int seq;
  ::std::string nodeId;
  ::std::string volumeId;
  ::std::string iqn;
  ::std::string instanceId;
  ::std::string device;
  ::TvmEvt1::SanInfo san;

  bool operator==(const DetachVolumeRequest&) const;
  bool operator!=(const DetachVolumeRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const DetachVolumeRequest&);

struct DetachVolumeResult
{
  DetachVolumeResult();
  Int seq;
  ::std::string nodeId;
  ::std::string volumeId;
  ::std::string instanceId;
  ::std::string device;
  ::std::string result;
  ::std::string state;
  Int detachTime;

  bool operator==(const DetachVolumeResult&) const;
  bool operator!=(const DetachVolumeResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const DetachVolumeResult&);

struct VmStat
{
  VmStat();
  ::std::string nodeId;
  ::std::string instanceId;
  Int memory;
  Int cpu;
  Long rxBytes;
  Long rxPackets;
  Long txBytes;
  Long txPackets;

  bool operator==(const VmStat&) const;
  bool operator!=(const VmStat&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const VmStat&);

struct RegisterNodeRequest
{
  RegisterNodeRequest();
  Int seq;
  ::std::string networkType;
  ::std::string nodeId;
  ::std::string hostname;
  ::std::string nodeIp;
  ::std::string nodeOs;
  ::std::string initiatorIqn;
  Int totalCpu;
  Long totalMem;
  Long totalDisk;

  bool operator==(const RegisterNodeRequest&) const;
  bool operator!=(const RegisterNodeRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const RegisterNodeRequest&);

struct RegisterNodeResult
{
  RegisterNodeResult();
  Int seq;
  ::std::string nodeId;
  ::TvmEvt1::SanInfo san;
  ::TvmEvt1::CephInfo ceph;
  ::TvmEvt1::StorageEngineInfo storageEninge;
  ::TvmEvt1::VSwitchs vswitchList;
  ::std::string bootImgLoc;
  ::std::string bootImgPoolName;
  ::std::string addrAssignMode;
  ::std::string result;

  bool operator==(const RegisterNodeResult&) const;
  bool operator!=(const RegisterNodeResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const RegisterNodeResult&);

struct UnRegisterNodeRequest
{
  UnRegisterNodeRequest();
  Int seq;
  ::std::string nodeId;

  bool operator==(const UnRegisterNodeRequest&) const;
  bool operator!=(const UnRegisterNodeRequest&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const UnRegisterNodeRequest&);

struct UnRegisterNodeResult
{
  UnRegisterNodeResult();
  Int seq;
  ::std::string nodeId;
  ::std::string result;

  bool operator==(const UnRegisterNodeResult&) const;
  bool operator!=(const UnRegisterNodeResult&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const UnRegisterNodeResult&);

}

#endif
