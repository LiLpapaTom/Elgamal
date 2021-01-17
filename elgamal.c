//Thomas Papaloukas, icsd14155
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <sys/types.h>
#include <unistd.h>

gmp_randstate_t stat; 

int main(void){
	
	char str[50]={'0'},buff[50]={'0'},array[2];
	int i,j=0,primetemp=0;
	unsigned long int ascii[50];
	long sd=0;
	mpz_t prime,seed,p,p2,p3,f,g,k,d,tmp,tmp2,p_1,gamma,delta,plain,decrypted;
	
	gmp_randinit(stat, GMP_RAND_ALG_LC, 120);
	mpz_init(prime);
	mpz_init(seed);
	mpz_init(p);
	mpz_init(p2);
	mpz_init(p3);
	mpz_init(f);
	mpz_init(p_1);
	mpz_init(g);
	mpz_init(k);
	mpz_init(d);
	mpz_init(tmp);
	mpz_init(tmp2);
	mpz_init(gamma);
	mpz_init(delta);
	mpz_init(plain);
	mpz_init(decrypted);
	
	srand( (unsigned) getpid() );
	sd = rand();
	mpz_set_ui(seed, sd);

	gmp_randseed(stat, seed);

	printf("Please enter your message: \n");
	fgets(str, 50, stdin);
	
	//Input to ascii
	printf("Input to ascii: \n");
	for(i=0;i<strlen(str);i++){
		ascii[i] = (unsigned long int) str[i];
		printf("%ld",ascii[i]);
	}
	puts("");
	
	for(i=0;i<strlen(str)*2;i+=2){
		sprintf(&buff[i], "%ld", ascii[j]/10);
		sprintf(&buff[i+1], "%ld", ascii[j]%10);
		j++;
	}
	//puts(buff);
	mpz_set_str(plain, buff, 10);
	//gmp_printf("Plain text: %Zd\n",plain);
	
	//Find prime p, based on probability function
	while(primetemp==0){
		mpz_urandomb(p2, stat, 200); //random to { 0 , 2^200-1 }
		primetemp = mpz_probab_prime_p(p2,10);	
	}
	gmp_printf("p2 = %Zd\n",p2);
	
	//Find prime p, in order to p = 2*p3 + 1
	do{
		mpz_nextprime(p3, p2);
		mpz_mul_ui(p, p3, 2);
		mpz_add_ui(p, p, 1);
		primetemp = mpz_probab_prime_p(p,10);
		mpz_set(p2, p3);

	}while(!primetemp);
	gmp_printf("p = %Zd\n",p);

	mpz_sub_ui(f, p, 1);
	gmp_printf("f = %Zd\n",f);

	//Generator
	mpz_set_ui(g ,1);
	do{
		mpz_add_ui(g, g, 1);
		mpz_div_ui(tmp, f, 2);
		mpz_powm(tmp2, g, tmp, p);
		primetemp = mpz_cmpabs_ui(tmp2,1);

		if(primetemp){
			mpz_div(tmp, f, p3);
			mpz_powm(tmp2, g, tmp, p);
			primetemp = mpz_cmpabs_ui(tmp,1);
		}
	}while(!primetemp);
	gmp_printf("%Zd is the generator \n",g);

	//Key creation
	//Find d, in order 1 <= d <= p-2 
	mpz_sub_ui(d, f, 1);
	mpz_urandomm(d, stat, d);// random to { 0 , p-2  }
	gmp_printf("%Zd \n",d);
	
	//g^k mod p
	mpz_powm(tmp, g, d, p);

	//Find k
	mpz_set_ui(seed, 5*sd);
	gmp_randseed(stat, seed);
	mpz_sub_ui(p_1, p, 1);	// p_1 = p - 1
	mpz_urandomm(k, stat, p_1);//Random k, based on state, in { 0, p-2 }
	gmp_printf("k = %Zd\n",k);

	//Encrypt
	mpz_powm(gamma, g, k, p);// gamma

	mpz_powm(tmp, gamma, d, p);//delta
	mpz_mul(delta, tmp, plain);
	mpz_mod(delta, delta, p);

	gmp_printf("Ciphertext :\ngamma = %Zd\ndelta = %Zd\n",gamma,delta);

	//Decrypt
	mpz_sub(tmp, f, d);// f-d
	mpz_powm(tmp2, gamma, tmp, p);// gamma^(-d) mod p
	//decrypt to plaintext
	mpz_mul(decrypted, tmp2, delta);
	mpz_mod(decrypted, decrypted, p);
	gmp_printf("Decrypted message: %Zd\n", decrypted);
	
	
	mpz_get_str(buff, 10, decrypted);
	j=0;
	for(i=0;i<strlen(buff);i+=2){
	  array[0] = buff[i];
	  array[1] = buff[i+1];
	  ascii[j] = atoi(array);
	  j++;
	}
	i=0;
	do{
		str[i] = (char) ascii[i];
		i++;
	}while(!ascii[i]);
	puts(str);
	
	mpz_clear(prime);
	mpz_clear(seed);
	mpz_clear(p);
	mpz_clear(p2);
	mpz_clear(p3);
	mpz_clear(f);
	mpz_clear(p_1);
	mpz_clear(g);
	mpz_clear(k);
	mpz_clear(d);
	mpz_clear(tmp);
	mpz_clear(tmp2);
	mpz_clear(gamma);
	mpz_clear(delta);
	mpz_clear(plain);
	mpz_clear(decrypted);
	

}
