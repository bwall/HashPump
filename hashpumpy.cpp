#include <Python.h>
#include <sstream>
#include <iomanip>
#include "Extender.h"

#define MODULE_NAME "hashpumpy"

/* Definitions for Python2/3 compatibility */
#if PY_MAJOR_VERSION >= 3
#   define INITERROR return NULL
#   define INITOK(m) return m
#   define PyInit_Signature(name) PyInit_##name
#   define BYTES_FORMAT "y#"
#else
#   define INITERROR return
#   define INITOK(m) return
#   define PyInit_Signature(name) init##name
#   define BYTES_FORMAT "s#"
#endif

typedef unsigned char BYTE;

vector<BYTE> StringToVector(BYTE * str);
void DigestToRaw(string hash, BYTE * raw);
Extender * GetExtenderForHash(string sig);

extern "C"
{
static PyObject *HashpumpError;


static PyObject *
hashpump(PyObject *self, PyObject *args)
{
    int key_length = 0;
    Extender * extender = NULL;
    const char* hexdigest=0, *original_data=0, *data_to_add=0;
    size_t hexdigest_size=0, original_data_size=0, data_to_add_size=0;


    //
    // Get the arguments and validate them
    //
    if (!PyArg_ParseTuple(args, "s#s#s#i",
                          &hexdigest, &hexdigest_size,
                          &original_data, &original_data_size,
                          &data_to_add, &data_to_add_size,
                          &key_length))
        return NULL;


    if(0 == hexdigest_size)
    {
        PyErr_SetString(HashpumpError, "hexdigest is empty");
        return NULL;
    }

    if(hexdigest_size % 2)
    {
        PyErr_SetString(HashpumpError, "hexdigest is invalid");
        return NULL;
    }

    if(0 == data_to_add_size)
    {
        PyErr_SetString(HashpumpError, "data_to_add is empty");
        return NULL;
    }

    extender = GetExtenderForHash(hexdigest);

    if(extender == NULL)
    {
        PyErr_SetString(HashpumpError, "unsupported hash size");
        return NULL;
    }

    //
    // Convert the digest from hexascii to binary
    //
    // Might be worthwhile to just call out to str.decode('hex')
    //
    size_t hash_bytes   = hexdigest_size/2;
    BYTE* original_hash = (BYTE*) malloc(hash_bytes);
    BYTE* new_hash      = (BYTE*) malloc(hash_bytes);

    DigestToRaw(hexdigest, original_hash);

    //
    // HashPump wants the messages as byte vectors
    //
    vector<BYTE> v_original_data(original_data, original_data + original_data_size);
    vector<BYTE> v_data_to_add(data_to_add, data_to_add + data_to_add_size);

    //
    // The second message is also returned as a similar vector
    //
    vector<BYTE> * v_new_message = NULL;

    //
    // Perform the extension attack to generate new_hash and v_new_message
    //
    v_new_message = extender->GenerateStretchedData(v_original_data,
                                                  key_length,
                                                  original_hash,
                                                  v_data_to_add,
                                                  &new_hash);


    //
    // We need to give the user back a hexdigest.
    // HashPump didn't implement this conversion, so we do it here.
    //
    string new_digest;
    stringstream ss;

    ss << hex << setfill('0');
    for(size_t i = 0; i < hash_bytes; i++)
    {
        ss << setw(2) << (unsigned) new_hash[i];
    }
    delete new_hash;

    new_digest = ss.str();

    //
    // Convert from byte-vector to string
    //
    string new_message = string(v_new_message->begin(), v_new_message->end());

    //
    // Return a tuple of (str, str)
    //
    return Py_BuildValue("s#" BYTES_FORMAT,
                         new_digest.c_str(), new_digest.size(),
                         new_message.c_str(), new_message.size());
}

#define MULTI_LINE_STRING(...) #__VA_ARGS__
static PyMethodDef HashpumpMethods[] = {
    {"hashpump",  hashpump, METH_VARARGS,
"hashpump(hexdigest, original_data, data_to_add, key_length) -> (digest, message)\n"
"\n"
"Arguments:\n"
"    hexdigest(str):      Hex-encoded result of hashing key + original_data.\n"
"    original_data(str):  Known data used to get the hash result hexdigest.\n"
"    data_to_add(str):    Data to append\n"
"    key_length(int):     Length of unknown data prepended to the hash\n"
"\n"
"Returns:\n"
"    A tuple containing the new hex digest and the new message.\n"
},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    MODULE_NAME,
    NULL,
    -1,
    HashpumpMethods,
    NULL,
    NULL,
    NULL,
    NULL
};
#endif

PyMODINIT_FUNC
PyInit_Signature(hashpumpy) (void)
{
    PyObject *m;

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&moduledef);
#else
    m = Py_InitModule(MODULE_NAME, HashpumpMethods);
#endif
    if (m == NULL)
        INITERROR;

    HashpumpError = PyErr_NewException("hashpumpy.error", NULL, NULL);
    Py_INCREF(HashpumpError);
    PyModule_AddObject(m, "error", HashpumpError);

    INITOK(m);
}
}
