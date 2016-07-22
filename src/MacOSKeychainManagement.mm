/*
 *  Copyright (c) 2015 Álan Crístoffer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

#import "MacOSKeychainManagement.h"

#import <CoreFoundation/CoreFoundation.h>
#import <CoreServices/CoreServices.h>
#import <Security/Security.h>

void RegisterCertificate(const QSslCertificate &cert, const QString &name)
{
    RemoveCertificate(name);

    CFDataRef         certData    = cert.toDer().toCFData();
    SecCertificateRef certificate = SecCertificateCreateWithData(kCFAllocatorDefault, certData);

    OSStatus status = SecCertificateAddToKeychain(certificate, nullptr);

    if ( ( (status == noErr) || (status == errKCDuplicateItem) ) && (name == "void.ca") ) {
        CFMutableArrayRef trustSettingMutArray = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
        SecTrustSettingsSetTrustSettings(certificate, kSecTrustSettingsDomainUser, trustSettingMutArray);
        CFRelease(trustSettingMutArray);
    }

    CFRelease(certData);
    CFRelease(certificate);
}

void RemoveCertificate(const QString &name)
{
    CFStringRef cfname = name.toCFString();

    const void *keys[] = {
        kSecClass,
        kSecAttrLabel,
        kSecReturnRef
    };

    const void *values[] = {
        kSecClassCertificate,
        cfname,
        kCFBooleanTrue
    };

    CFDictionaryRef query       = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 3, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFTypeRef       certificate = nullptr;
    SecIdentityRef  identity    = nullptr;
    SecKeyRef       pkey        = nullptr;

    SecItemCopyMatching(query, &certificate);
    if ( certificate != nullptr ) {
        SecIdentityCreateWithCertificate(nullptr, reinterpret_cast<SecCertificateRef> ( const_cast<void *> (certificate) ), &identity);
        if ( identity != nullptr ) {
            SecIdentityCopyPrivateKey(identity, &pkey);
            if ( pkey != nullptr ) {
                SecKeychainItemDelete( reinterpret_cast<SecKeychainItemRef> (pkey) );

                CFRelease(pkey);
            }

            CFRelease(identity);
        }

        SecKeychainItemDelete( reinterpret_cast<SecKeychainItemRef> ( const_cast<void *> (certificate) ) );
        CFRelease(certificate);
    }

    CFRelease(cfname);
}
