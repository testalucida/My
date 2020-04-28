#include <my/Crypt.h>

#ifdef WIN32
#include <Windows.h>
#include <WinCrypt.h>
#endif

namespace my {

CryptRC Crypt::encrypt( CryptData &cdata ) const {
	CryptRC crc;
#ifdef WIN32
	//Declare and initialize variables.
	HCRYPTPROV hCryptProv = 0;        // handle for a cryptographic provider context
	crc.rc = CryptAcquireContext( &hCryptProv,      // handle to the CSP
							       NULL,         // container name
								   MS_DEF_PROV,      // use default CSP
								   PROV_RSA_FULL,      // provider type
								   0 );  // flag values

	//----------------------------------------------------------

	HCRYPTHASH hHash;
	HCRYPTKEY hKey; // handle for a cryptographic key

	// Obtain handle to hash object.
	crc.rc = CryptCreateHash(	hCryptProv,             // Handle to CSP obtained earlier
							CALG_MD5,          // Hashing algorithm
							0,                 // Non-keyed hash
							0,                 // Should be zero
							&hHash);           // Variable to hold hash object handle

	// Hash data.
	crc.rc = CryptHashData(	hHash,             // Handle to hash object
							cdata.pPwd,         // Pointer to password
							cdata.pwdLen,  // Length of data
							0);                // No special flags


	// Create key from specified password.
	crc.rc = CryptDeriveKey( hCryptProv,               // Handle to CSP obtained earlier.
						 CALG_RC4,            // Use a stream cipher.
						 hHash,               // Handle to hashed password.
						 CRYPT_EXPORTABLE,    // Make key exportable.
						 &hKey );              // Variable to hold handle of key.

	//----------------------------------------------------------

	// Have API return us the required buffer size.
	ulong size = cdata.inDataLen;
	crc.rc = CryptEncrypt( hKey,            // Key obtained earlier
						   0,               // No hashing of data
						   TRUE,            // Final or only buffer of data
						   0,               // Must be zero
						   NULL,            // No data yet, simply return size
						   &size,         // Size of data
						   size );         // Size of block

	// We now have a size for the output buffer, so create buffer.
	uchar *pEncryptData = new uchar[size];
	memcpy( pEncryptData, cdata.pInData, cdata.inDataLen );

	// Now encrypt data.
	crc.rc = CryptEncrypt( hKey,            // Key obtained earlier
						   0,               // No hashing of data
						   TRUE,            // Final or only buffer of data
						   0,               // Must be zero
						   pEncryptData,         // Data buffer
						   &size,         // Size of data
						   size );         // Size of block
	cdata.pOutData = pEncryptData;
	cdata.outDataLen = size;

	//-----------------------------------------------------------

	// Release hash object.
	crc.rc = CryptDestroyHash(hHash);

	// Release key object.
	crc.rc = CryptDestroyKey(hKey);

	crc.rc = CryptReleaseContext( hCryptProv, 0 );
#endif
	return crc;
}


CryptRC Crypt::decrypt( CryptData &cdata ) const {
	CryptRC crc;
#ifdef WIN32
	//Declare and initialize variables.
	HCRYPTPROV hCryptProv = 0;        // handle for a cryptographic provider context
	int rc = CryptAcquireContext( &hCryptProv,      // handle to the CSP
							       NULL,         // container name
								   MS_DEF_PROV,      // use default CSP
								   PROV_RSA_FULL,      // provider type
								   0 );  // flag values

	//----------------------------------------------------------

	HCRYPTHASH hHash;
	HCRYPTKEY hKey; // handle for a cryptographic key
	ulong passwordLen = 8;
	uchar pwd[8] = {'S', 'o', 'm', 'm', 'e', 'r', '1', '3'};

	// Obtain handle to hash object.
	rc = CryptCreateHash(	hCryptProv,             // Handle to CSP obtained earlier
							CALG_MD5,          // Hashing algorithm
							0,                 // Non-keyed hash
							0,                 // Should be zero
							&hHash);           // Variable to hold hash object handle

	// Hash data.
	rc = CryptHashData(		hHash,             // Handle to hash object
							cdata.pPwd,         // Pointer to password
							cdata.pwdLen,      // Length of data
							0);                // No special flags


	// Create key from specified password.
	rc = CryptDeriveKey( hCryptProv,               // Handle to CSP obtained earlier.
						 CALG_RC4,            // Use a stream cipher.
						 hHash,               // Handle to hashed password.
						 CRYPT_EXPORTABLE,    // Make key exportable.
						 &hKey );              // Variable to hold handle of key.


	//----------------------------------------------------------

	ulong size = cdata.inDataLen;
	uchar *pDecryptData = new uchar[size];
	memcpy( pDecryptData, cdata.pInData, size );

	rc = CryptDecrypt( 	hKey,            // Key obtained earlier
						0,               // No hashing of data
						TRUE,            // Final or only buffer of data
						0,               // Must be zero
						pDecryptData,      // Data buffer
						&size );  // Size of block

	cdata.pOutData = pDecryptData;
	cdata.outDataLen = size;
	//------------------------------------------------------------

	// Release hash object.
	rc = CryptDestroyHash(hHash);

	// Release key object.
	rc = CryptDestroyKey(hKey);

	rc = CryptReleaseContext( hCryptProv, 0 );
#endif
	return crc;
}


}
