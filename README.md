# dscat-cuitool

The dscat scrambles bits of a stream or a file and output it in multiple files as pieces.

It makes a file difficult to guess the contents and formats of the file by a piece.

(To use it for security purposes, it must be used together with any encrypt/decrypt tools.)

## Installation

Just download from the following link.

- MacOS X : https://github.com/dscat/resources/blob/master/dscat.tar.gz
    - Do not share direct link. Share this page.

Unzip and move to your $PATH directory.

## Usage

### Scatting a file

#### Example: From stdin, 4 pieces
```
$ cat /tmp/scatter.data | dscat -s -i -c 4 -p /tmp/p1,/tmp/p2,/tmp/p3,/tmp/p4
$
$ ls -al /tmp/
p1
p2
p3
p4
scatter.data
```

### Gathering files

#### Example: From 4 pieces
```
$ dscat -g -c 4 -p /tmp/p1,/tmp/p2,/tmp/p3,/tmp/p4 -o /tmp/gather.data
$
$ ls -al /tmp/
gather.data
p1
p2
p3
p4
scatter.data
```
- Cannot restore the source file if all of the pieces isn't gathered.
- Automatically verify the match of gather.data hash-value and scatter.data.

### To secure your file, encryption/scatting and gathering/decryption with 4 pieces
#### Scatting
```
$ shasum -a 256 /tmp/scatter.data
76d7814e64fd83941d128aaeb3178e5b82661d445857df41ef9bfadf585ac775  /tmp/scatter.data
$ cat /tmp/scatter.data | openssl enc -e -aes-128-cbc -pass pass:your_pass | dscat -s -i -c 4 -p /tmp/p1,/tmp/p2,/tmp/p3,/tmp/p4
```
#### Gathering
```
$ dscat -g -c 4 -p /tmp/p1,/tmp/p2,/tmp/p3,/tmp/p4 | openssl enc -d -aes-128-cbc -pass pass:your_pass > /tmp/gather.data
$ shasum -a 256 /tmp/gather.data
76d7814e64fd83941d128aaeb3178e5b82661d445857df41ef9bfadf585ac775  /tmp/gather.data
```

### Others

#### man
```
SYNOPSIS
        ./dscat [-s|-g] [-c <pieces>] [-p <pieces files>] [-i] [-o <output file>] [-v] [-t]

OPTIONS
        -s, --scatting|-g, --gathering
                    mode

        <pieces>    for scatting, count of pieces(1-8).

        <pieces files>
                    pieces file(s) comma split.

        -i, --stdin input from stdin for -s.

        <output file>
                    file name for -g.

        -v, --verbose
                    verbose mode.

        -t, --test  test mode (no output results).
```

#### Verbose mode (-v)
```
$ echo "abcde" | ./dscat -s -i -c 4 -p /tmp/p1,/tmp/p2,/tmp/p3,/tmp/p4 -v
(2018/08/18 19:28:43) [-] 🐈 scatter v1.0
(2018/08/18 19:28:43) [-] Copytight (c) 2018 https://github.com/dscat/cuitool
(2018/08/18 19:28:43) [ ] Starting scatting mode with 4 pieces.
(2018/08/18 19:28:43) [ ] from 4 files.
(2018/08/18 19:28:43) [ ] Scatted #1 : e892b946b17f9e3691b9d10d302656483cfdff5dc360c024f4fb37e244e3c572
(2018/08/18 19:28:43) [ ] Writing /tmp/p1...
(2018/08/18 19:28:43) [ ] Scatted #2 : 983efd9589bbeaf6cd9eb9ee7bdcac23a6329d5c6fe1738fe9fb8a5107452eb0
(2018/08/18 19:28:43) [ ] Writing /tmp/p2...
(2018/08/18 19:28:43) [ ] Scatted #3 : 6d344d76c31fd6c88c6d7c936ca1ed2cfd59646d9e8bb2176e5732be1725860a
(2018/08/18 19:28:43) [ ] Writing /tmp/p3...
(2018/08/18 19:28:43) [ ] Scatted #4 : a90b2ead36c1ea84eb5d466e16e3e00d6e689ae3c6e648bb418ee2448ecc096b
(2018/08/18 19:28:43) [ ] Writing /tmp/p4...
(2018/08/18 19:28:43) [-] 🐈 completed!
$
$ ./dscat -g -c 4 -p /tmp/p1,/tmp/p2,/tmp/p3,/tmp/p4 -v
(2018/08/18 19:44:15) [-] 🐈 scatter v1.0
(2018/08/18 19:44:15) [-] Copytight (c) 2018 https://github.com/dscat/cuitool
(2018/08/18 19:44:15) [ ] Starting gathering mode with 4 pieces.
(2018/08/18 19:44:15) [ ] Loaded #1 from /tmp/p1. ( 42 byte(s). )
(2018/08/18 19:44:15) [ ] Loaded #2 from /tmp/p2. ( 42 byte(s). )
(2018/08/18 19:44:15) [ ] Loaded #3 from /tmp/p3. ( 42 byte(s). )
(2018/08/18 19:44:15) [ ] Loaded #4 from /tmp/p4. ( 42 byte(s). )
(2018/08/18 19:44:15) [ ] Gathered 6 byte(s) file.
abcde
(2018/08/18 19:44:15) [-] 🐈 completed!
```
