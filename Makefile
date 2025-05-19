echo 'CC = gcc' > Makefile
echo 'CFLAGS = -Wall -Wextra' >> Makefile
echo '' >> Makefile
echo 'all: bank_server bank_client' >> Makefile
echo '' >> Makefile
echo 'bank_server: bank_server.c bank_common.h' >> Makefile
echo -e '\t$(CC) $(CFLAGS) -o bank_server bank_server.c' >> Makefile
echo '' >> Makefile
echo 'bank_client: bank_client.c bank_common.h' >> Makefile
echo -e '\t$(CC) $(CFLAGS) -o bank_client bank_client.c' >> Makefile
echo '' >> Makefile
echo 'clean:' >> Makefile
echo -e '\trm -f bank_server bank_client' >> Makefile
echo '' >> Makefile
echo '.PHONY: all clean' >> Makefile