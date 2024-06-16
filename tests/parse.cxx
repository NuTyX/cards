#include <string>
#include <cstring>
#include <vector>

void parse(std::string& s, std::string k, std::vector<std::string>& content) {
    std::string::size_type p;
    bool found = false;
    for (auto i : content) {
        if (i[0] == '#')
            continue;

        p = i.find(k);
        if (p != std::string::npos) {
            s += i.substr(p + k.size());
            found = true;
            p = s.find(')');
            if (p != std::string::npos) {
                s = s.substr(0,p);
                break;
            }
            continue;
        }
        if (found) {
            p = i.find(')');
            if (p != std::string::npos) {
                s += i.substr(0,p);
                break;
            } else
                s = s + " " + i;
        }
    }
	while ((p = s.find("\t")) != std::string::npos) {
		s = s.replace(p, 1, " ");
    }
}
int main() {
    FILE* file = fopen("Pkgfile","r");
    if(!file) {
        printf("fail");
        return -1;
    }
    std::vector<std::string> content;
    char line[BUFSIZ];
    while (fgets(line,BUFSIZ,file)) {
        line[strlen(line)-1]='\0';
        content.push_back(line);
    }
    fclose(file);

    std::string s;
    parse(s,"makedepends=(",content);
    s += " ";
    parse(s,"run=(",content);

    int beg = 0;
    int pos = s.length();

    while ( pos > -1 ){
        if (isspace(s[pos])) {
            pos--;
            if(isspace(s[pos])) {
                beg=pos;
                while ( pos > -1 && isspace(s[pos])) {
                    pos--;
                }
                s.erase(pos + 1, beg - pos);
            }
        }
        pos--;
    }
    printf("%s\n",s.c_str());
    return 0;
}
