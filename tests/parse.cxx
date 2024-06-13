#include <string>
#include <cstring>

int main() {

    FILE* file = fopen("Pkgfile","r");
    
    if(!file) {
        printf("fail");
        return -1;
    }
    char line[BUFSIZ];
    std::string s;
    while (fgets(line,BUFSIZ,file)) {
        line[strlen(line)-1]=' ';
        s += line;
    }
    
    fclose(file);
    
    /*
     * Get the dependencies
     * Need to be done for both makedepends and run variables
     */
    std::string::size_type p;
    p=s.find("makedepends=(");
    if (p != std::string::npos) {
        s=s.substr(p+13); // 13 = length of "makedepends=("
        p=s.find(')');
        if (p != std::string::npos) {
            s = s.substr(0,p);
        }
    }
    /*
     * Get rid of extra spaces
     */

    int beg = 0;
    int pos = s.length();;

    while ( pos > -1 ){
        if (isspace(s[pos])) {
            pos--;
            if(isspace(s[pos])) {
                beg=pos+1;
                while ( pos > -1 && isspace(s[pos])) {
                    pos--;
                }
                s.erase(pos,beg-pos);
            }
        }
        pos--;
    }
    printf("%s\n",s.c_str());
    
        
    return 0;
}
