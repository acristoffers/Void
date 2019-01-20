# VOID

Void is an encrypted store. You can encrypt/decrypt files from/to the store. It
supports viewing images and editing text inside the application, without having
to decrypt to disk.

Videos are also played from memory, but due to a bug/limitation in Qt you will
have to type your password if you're on a Mac to allow the creation and use of a
temporary certificate. With this, a server is created and the video is securely
transfered to the player.

You can also add tags and comments to the files, and search will lookup file
path, tags and comments.

Encryption is carried out by NSS/NSPR using AES_GCM_256, SHA512, PKCS5_PBKDF2,
HMAC_SHA512 with a cost of 250000. The store itself has a key derived from your
password and each file has its own random key too. In disk all encrypted files
get a salted-hash name, 128 digits long. Large files (50MB) are divided into
smaller parts.

Remember: an application is just as secure as the system it runs on. If your
operating system can not be trusted, there's nothing that can be done to keep
you safe.

## Screenshots

![](ScreenShots/Void1.png)
![](ScreenShots/Void2.png)
![](ScreenShots/Void3.png)
![](ScreenShots/Void4.png)
![](ScreenShots/Void5.png)
![](ScreenShots/Void6.png)
![](ScreenShots/Void7.png)
![](ScreenShots/Void8.png)

## LICENSE

Copyright (c) 2015 Álan Crístoffer

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
