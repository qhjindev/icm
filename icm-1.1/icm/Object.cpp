#include "icm/Object.h"
#include "icm/ServerRequest.h"

DispatchStatus
Blobject::__dispatch(ServerRequest& request) {
/*
  vector < Byte > inParams;
  vector < Byte > outParams;
  Int sz = in.is()->getReadEncapsSize();
  in.is()->readBlob(inParams, sz);
  bool ok = ice_invoke(inParams, outParams, current);
  in.os()->writeBlob(outParams);
  if (ok) {
    return DispatchOK;
  } else {
    return DispatchUserException;
  }
*/

  bool ok = icm_invoke(request);
  request.initReply();
  if (ok)
    return DispatchOK;
  else
    return DispatchUserException;
}
