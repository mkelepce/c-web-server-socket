#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>

char sayfa[]= "HTTP/1.1 200 OK\r\n"
"Content-Type:text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>Clamp Server</title>\r\n"
"<style>body { background-color: #FFF }</style></head>\r\n"
"<body><center><h1>Clamp Server Running</h1><br>\r\n"
"<img src=\"doctest.jpg\"></center></body></html>\r\n";

int main(int argc, char *argv[]){
	
	
	struct sockaddr_in sunucu_addr, client_addr; //sockaddr_in türünde server ve client
	socklen_t sin_len = sizeof(client_addr); //client_addr sizeof operatoru ile boyutu 
	int a_server, a_client;
	char mesaj[2048]; //2 byte'lık alan
	int resim;
	int on = 1;
	a_server = socket(AF_INET, SOCK_STREAM, 0); // IPV4 ve TCP 
	//if kosulu ile hata kontrolu
	if(a_server < 0){ //eksi bir donerse hata var demektir
		perror("socket"); //hata yaz
	exit(1); //basarisizlikla sonuclandigini belirmek icin exit islevi 
	}
	setsockopt(a_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)); // sizeof int turu boyutu
	
	sunucu_addr.sin_family = AF_INET; //ipv4
	sunucu_addr.sin_addr.s_addr= INADDR_ANY; //kendi ipsini
	sunucu_addr.sin_port = htons(8010); //host to network short ile soket portu 8010

	//if kosulu ile hata var mı kontrol ediyoruz
	if(bind(a_server, (struct sockaddr *) &sunucu_addr, sizeof(sunucu_addr)) ==-1){ //bind islevinin geri donus degeri -1 ise hata var demektir
	perror("bind");//hata yaz
	close(a_server);//serveri kapat
	exit(1); // basarisiz durumlarda exit 1
	}
	
	//if kosulu ile listen hata var mı kontrol ediyoruz
	if(listen(a_server, 10) == -1){ //10 sayisi gelecek isteklerde kuyruk sayisi
	perror("listen"); //hata yaz
	close(a_server); //serveri kapat
	exit(1); //basarisiz durum exit 1
	}
	
	while(1){
		a_client = accept(a_server,(struct sockaddr *) &client_addr, &sin_len); //accept istekleri kabul et
		if(a_client == -1){
			perror("baglanti aliniyor..\n");
			continue;
		}
		printf("client baglaniyor..\n");
		
		if(!fork()){ //cocuk process 
			close(a_server);
			memset(mesaj,0,2048); // mesaji 2 bytelık alana yaz
			read(a_client,mesaj,2047); //eksi bir oku
			
			printf("%s\n", mesaj); //mesaji yazdir

			if(!strncmp(mesaj, "GET /favicon.ico",16)){ //http get metodu ile favicon ekle
			resim = open("favicon.ico", O_RDONLY); //favicon ac
			sendfile(a_client, resim, NULL, 4000);	 //favicon'u server'a baglanan client a goster
			close(resim);		
			}
			else if(!strncmp(mesaj, "GET /doctest.jpg" ,16)){ //sayfaya resim.jpg ekle
			resim = open("doctest.jpg", O_RDONLY);
			sendfile(a_client, resim, NULL, 6000); //resimi client a gonder
			close(resim);
			}
			else
				write(a_client,sayfa,sizeof(sayfa) -1);
							
			
			close(a_client);
			printf("kapaniyor..\n");
			exit(0);
			
		}
	close(a_client);
	}

return 0;
}
