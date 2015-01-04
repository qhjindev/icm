#ifndef ICM_ERROR_H
#define ICM_ERROR_H

const int ICM_UNKNOWN = 50000;          // the unknown exception
const int ICM_BAD_PARAM = 50000;        // an invalid parameter was passed
const int ICM_NO_MEMORY = 50000;        // memory allocation failure
const int ICM_IMP_LIMIT = 50000;        // violated implementation limit
const int ICM_COMM_FAILURE = 50000;     // communication failure
const int ICM_INV_OBJREF = 50000;       // invalid object reference
const int ICM_OBJECT_NOT_EXIST = 50000; // no such object
const int ICM_NO_PERMISSION = 50000;    // no permission for operation
const int ICM_INTERNAL = 50000;         // ORB internal error
const int ICM_MARSHAL = 50000;          // error marshaling param/result
const int ICM_INITIALIZE = 50000;       // ORB initialization failure
const int ICM_NO_IMPLEMENT = 50000;     // implementation unavailable
const int ICM_BAD_TYPECODE = 50000;     // bad typecode
const int ICM_BAD_OPERATION = 50000;    // invalid operation
const int ICM_NO_RESOURCES = 50000;     // out of resources for request
const int ICM_NO_RESPONSE = 50000;      // response not yet available
const int ICM_PERSIST_STORE = 50000;    // persistent storage failure
const int ICM_BAD_INV_ORDER = 50000;    // routine invocations out of order
const int ICM_TRANSIENT = 50000;        // transient error, try again later
const int ICM_FREE_MEM = 50000;         // cannot free memory
const int ICM_INV_IDENT = 50000;        // invalid identifier syntax
const int ICM_INV_FLAG = 50000;         // invalid flag was specified
const int ICM_INTF_REPOS = 50000;       // interface repository unavailable
const int ICM_BAD_CONTEXT = 50000;      // error processing context object
const int ICM_OBJ_ADAPTER = 50000;      // object adapter failure
const int ICM_DATA_CONVERSION = 50000;  // data conversion error
const int ICM_INV_POLICY = 50000;       // invalid policies present
const int ICM_REBIND = 50000;           // rebind needed
const int ICM_TIMEOUT = 50000;          // operation timed out
const int ICM_TRANSACTION_UNAVAILABLE = 50000; // no transaction
const int ICM_TRANSACTION_MODE = 50000;        // invalid transaction mode
const int ICM_TRANSACTION_REQUIRED = 50000;    // operation needs transaction
const int ICM_TRANSACTION_ROLLEDBACK = 50000;  // operation was a no-op
const int ICM_INVALID_TRANSACTION = 50000;     // invalid TP context passed

const int ICM_UNKNOWN_ERROR = 50000;
const int ICM_INVOCATION_RECV_REQUEST = 50000;
const int ICM_TIMEOUT_CONNECT = 50000;
const int ICM_TIMEOUT_SEND = 50001;
const int ICM_TIMEOUT_RECV = 50001;

// add by rom start
const int ICM_INVOCATION_START_FAILED = 50002;
const int ICM_INVOCATION_PREPAREHEADER_FAILED = 50003;
const int ICM_INVOCATION_INVOKE_FAILED = 50004;
// add by rom end.
#endif //ICM_ERROR_H
