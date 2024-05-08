### Full Name

### Student ID

### EXTRA CREDIT
Yes. I designed an algorithm which using 'randomly shaffling + mapping' strategy to encrypt username and passcode.
### Submission Files
#### Submission Files
member.txt : generated from member_unencrypted.txt for serverM to load.
member_unencrypted : same as the original file.
client.cpp : client code which is only used under customized encrypt algorithm.
serverM.cpp : serverM code. Used with member.txt.
serverS.cpp : serverS code. Used with single.txt.
serverU.cpp : serverU code. Used with suite.txt.
serverD.cpp : serverD code. Used with double.txt.
#### Format of messages
Mixed messages: username and password are concatenated and delimited by a comma; roomcode and action are concatenated and delimited by a comma.
Single messages: itself.
#### Idiosyncrasy
Exit only by "ctrl+c", details are provided in "By the way".
#### Reused Code
Yes. *get_in_addr();getaddrinfo();getsockname();freeaddrinfo();inet_ntop();shutdown();

#### Submission Structure
##### ./ file : 
contains serverM.cpp, serverS.cpp, serverU.cpp, serverD.cpp, client.cpp(For extra credits only) and Makefile.
##### ./shift file : 
generateNewUser.cpp and Makefile. The generateNewUser.cpp is used to generate member.txt from member_unencrypted.txt when you decide to test encrypted service. Please enter "./generateNewUser" to generate member.txt after it gets complied from "make". I provide member.txt which is generated from original member_unencrypted.txt, if you want to test encrypt algorithm for the extra credit, please replace it with the member.txt in the root file.
##### ./Un
Contains client.cpp which using the encrypt algorithm mentioned in the homework description.
##### ./Unified client
"./client -en": customized encrypt algorithm. After execute "make", please enter "./serverM", "./serverD", "./serverS", "./serverU", "./cilent -en" in order. 

"./client -un": original encrypt algorithm. After execute "make", please enter "./serverM", "./serverD", "./serverS", "./serverU", "./cilent -un" in order. 

After execute "make", please enter "./cilent -en", "./serverD", "./serverS", "./serverU", "./cilent" in order.
##### customized encrypt algorithm
'randomly shaffle characters + mapping': next time, client and server only need to reach an agreement of shaffle seed, then it would be easily for them to encode and decode, the problem is this kind of encoding algorithm is just like the original one, they could be easily decoded by others through statistical methods. However, compared with the original encryption method, we can add a byte to unify the next encrypted seed every time we send a message, in fact, only one of server and client needs to dominant the selection of seed, which can easily solve the problem of statistical decryption. 

1.Initialization:
It initializes a string allCharacters containing all the characters that can be used for encryption, including uppercase letters, lowercase letters, and digits.
It sets a seed value for the random number generator to ensure consistent results each time the function is called.

2.Shuffling Characters:
It shuffles the characters in allCharacters using the std::shuffle function. This randomizes the order of characters, ensuring a different mapping each time the function is called.

3.Mapping Characters:
It creates a mapping between each character in allCharacters and the character at the next position in the shuffled string. For example, if 'A' is shuffled to 'X', then 'A' will be mapped to 'X'.
This mapping is stored in the charMap unordered map.

4.Encryption:
It iterates over each character in the plaintext string.
For each character, it checks if it exists in the charMap. If it does, it replaces the character with its mapped value.
This process effectively encrypts the plaintext using the shuffled character set.

5.Return:
It returns the ciphertext, which is the plaintext string encrypted using the shuffled character set.

#### By the way
"make all" or "make" are all feasible.

I get a little bit confused about command line input. Therefore, I provide three version of client.cpp. The first one which is used as I said before, like using "./client -en" to execute with customized encrypt algorithm. 

In addition, in the homework description, there is no specific way to deal with other edge cases or errors, such as the wrong instruction (D219, Watch) or the wrong room type (X211), I have added some unified handlers, which will be handled accordingly in client and serverM, and give the user some text form feedback, instead of directly shutting down the system, in my opinion, this is more in line with the reality. The user can reserve a room and remain in the service, as logically the user can still query.

If you find log in problem, please re-generate data from generateNewUser.cpp.
