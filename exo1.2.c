// sans #include <stdio.h>, le compilateur ne sait pas ce qu'est "printf"
// lors de la compilation, gcc affiche des warnings
// gcc fait alors une "implicit declaration" : il suppose une déclaration de printf
// cette supposition peut être incorrecte
// le programme peut fonctionner ici, mais le code n'est pas correct

int main(void){
	printf("Hello world!\n");
}

