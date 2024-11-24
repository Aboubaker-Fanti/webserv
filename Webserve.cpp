#include "Webserve.hpp"

/*          *************** canonical form ***************          */

Webserve::Webserve(const Webserve& other) {
    *this = other;
}

Webserve& Webserve::operator=(const Webserve& other){
    (void)other;
    return *this;
}


Webserve::Webserve() {
}

Webserve::~Webserve() {
}

Webserve::Webserve(std::ifstream& input) {
    fanti = 0;
    server_nbr = 0;
    std::string last_string = "";
    while (std::getline(input, buffur)) {
        if (check_if_full(buffur) == 0)
            fanti++;
        if (!buffur.empty()){
            check_configFile_error(buffur);
            if (last_string == buffur)
                throw std::range_error("config file not valide check deplicate;");
            last_string = buffur;
        }
    }
    if (fanti == 0)
        throw std::range_error("config file not valide please follow rules");
    for(size_t i = 0; i < server_nbr; i++){
        servers.push_back(Server());
    }
    nbr_of_serv = server_nbr;
    input.clear();
    input.seekg(0, input.beg);
    size_t nbr_serv = 0;
    server_nbr = 0;
    location_nbr = 0;
    while (std::getline(input, buffur)) {
        if (!buffur.empty()){
            std::string String;
            int i = skip_space_tap(buffur);
            size_t pos = buffur.find_first_of(':');
            String = buffur.substr(i, pos - i);
            check_file_input(String);
            if (String == "- server" || String == "- path") {
                if (String == "- path")
                    location_nbr++;
                if (String == "- server") {
                    server_nbr++;
                    if (server_nbr > nbr_serv) {
                        if (server_nbr != 1){
                            servers[server_nbr - 2].set_location(location_nbr);
                            servers[server_nbr - 2].set_location_nbr(location_nbr);
                        }
                        location_nbr = 0;
                    }
                }
            }
        }
    }
    servers[server_nbr - 1].set_location(location_nbr);
    servers[server_nbr - 1].set_location_nbr(location_nbr);
    input.clear();
    input.seekg(0, input.beg);
    fill_atripute(input);
    fill_vector_name_id();
    fill_content_type();
    names_file();

    check_root_with_error();
    check_error_pages();
}

/*      *************** Atripute setter ***************     */

 void Webserve::check_root_with_error(){

 }

std::vector<Server> Webserve::getServers() {
    return servers;
}

size_t Webserve::get_servers_nbr(){
    return (nbr_of_serv);
} 

int Webserve::skip_space_end(std::string str) {
    if (str == "    locations:")
        return (str.size());
    for(int i = str.size() - 1; str[i]; i--){
        if (str[i] != ' ' && str[i] != '\t')
            return (i);
    }
    return(0);
}

/*      *************** mumber function ***************     */

void Webserve::check_configFile_error(std::string str) {
    std::string String;
    std::string Str;

    int i = skip_space_tap(str);
    size_t pos = str.find_first_of(':');
    String = str.substr(i, pos - i);

    if (pos > str.size() && String[0] != '#')
        throw std::range_error("config file not valide please follow rules");
    if (String != "- server" && server_nbr == 0)
        throw std::range_error("config file not valide please follow rules");
    if (String == "- server" || String == "locations") {
        if (String == "- server")
            server_nbr++;
        if (String == "locations")
            location_nbr++;
        if (str[pos+1])
            throw std::range_error("config file not valide please follow rules");
    }
    else if (str[pos+1] != ' ' || skip_space_tap(str.substr(pos + 1)) == 0)
        throw std::range_error("config file not valide please follow rules");
    if (String == "client_max_body_size" || String == "port") {
                Str = str.substr(pos + 2);
        for (int i = 0; Str[i]; i++) {
            if (!isdigit(Str[i]) && Str[i] != ' ')
                throw std::range_error("config file not valide please follow rules");
        }
    }
}

void Webserve::check_file_input(std::string str)  {    
    std::string name[] = {"upload_pass", "upload_pass_args", "host","- server","port","server_name","client_max_body_size","root","index","locations","- path","allow_methods","autoindex","cgi","cgi-ext","return", "404_error_page", "403_error_page", "400_error_page", "402_error_page", "401_error_page", "413_error_page", "501_error_page", "505_error_page", "204_error_page", "412_error_page", "405_error_page", "500_error_page", "502_error_page"};
    int d = 0;
    for(int i = 0; i < 29 ; i++) {
        if (str == name[i]) {
            d = 1;
        }
    }
    if (d != 1 && str[0] != '#')
        throw std::range_error("config file not valide please follow rules");
}

int Webserve::check_if_full(std::string input){
    for (size_t i = 0; i < input.size(); i++){
        if (input[i] != ' ' && input[i] != '\t' && input[i] != '\n')
            return (0);
        if (input[i] == '\n')
            return (1);
    }
    return (1);
}

int check_file_exist(std::string path){
    const char* dir = path.c_str();
    struct stat sb;
    if (stat(dir, &sb) == 0 && !(sb.st_mode & S_IFDIR))
        return (1);
    return (-1);
}

void Webserve::check_error_pages(){
    for (size_t i = 0; i < nbr_of_serv; i++){
        if(servers[i].get_502_page().empty() || servers[i].get_500_page().empty() || servers[i].get_405_page().empty() || servers[i].get_412_page().empty() || servers[i].get_404_page().empty() || servers[i].get_403_page().empty() || servers[i].get_400_page().empty() || servers[i].get_402_page().empty() || servers[i].get_401_page().empty() || servers[i].get_413_page().empty() || servers[i].get_501_page().empty() || servers[i].get_505_page().empty() || servers[i].get_204_page().empty())
            throw std::range_error("config file error page not valide;");
        if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_403_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_400_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_402_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_404_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_401_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_413_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_501_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_505_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_204_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_412_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_405_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_500_page()) != 1)
            throw std::range_error("config file error page not valide;");
        else if (check_file_exist(servers[i].get_root() + "/" + servers[i].get_502_page()) != 1)
            throw std::range_error("config file error page not valide;");
        
        servers[i].check_index_valid();
    }

    for (size_t t = 0; t < this->nbr_of_serv; t++){
        servers[t].set_error_in_map();
        if (servers[t].get_port().empty())
            throw std::range_error("config file error check your server port;");
        if (servers[t].get_host().empty())
            throw std::range_error("config file error check your server host;");
        if (servers[t].get_server_name().empty())
            throw std::range_error("config file error check your server server name;");
        if (servers[t].get_client_max_size().empty())
            throw std::range_error("config file error check your server server client body size;");

    }
}

void Webserve::check_otheres_error(){
    std::vector<Server> ser = getServers();
    for (size_t i = 0;i < get_servers_nbr(); i++){
        if (ser[i].get_host().empty())
            throw std::range_error("there is invalid server name?");
        if (ser[i].get_port().empty())
            throw std::range_error("there is invalid server name?");
        if (ser[i].get_root().empty())
            throw std::range_error("there is invalid server name?");
    }
}

void Webserve::fill_atripute(std::ifstream& input) {
    size_t server = 0;
    std::string end_str;
    size_t location = 0;
    while (std::getline(input, buffur)) {
        size_t i = skip_space_tap(buffur);
        size_t pos = buffur.find_first_of(':');
        std::string str = buffur.substr(i, pos - i);
        if (!buffur.empty()){
            if (str == "- server"){
                server++;
                while (std::getline(input, buffur)){
                    i = skip_space_tap(buffur);
                    size_t pos = buffur.find_first_of(':');
                    std::string str = buffur.substr(i, pos - i);
                    int f = skip_space_end(buffur);
                    if (str != "locations")
                        end_str = buffur.substr(pos + 2, f - (pos + 1));
                    if (str == "port")
                        servers[server - 1].set_port(end_str, i);
                    else if (str == "host")
                        servers[server - 1].set_host(end_str, i);
                    else if (str == "server_name")
                        servers[server - 1].set_server_name(end_str, i);
                    else if (str == "client_max_body_size")
                        servers[server - 1].set_client_max_size(end_str, i);
                    else if (str == "404_error_page")
                        servers[server - 1].set_404_page(end_str, i);
                    else if (str == "403_error_page")
                        servers[server - 1].set_403_page(end_str, i);
                    else if (str == "400_error_page")
                        servers[server - 1].set_400_page(end_str, i);
                    else if (str == "402_error_page")
                        servers[server - 1].set_402_page(end_str, i);
                    else if (str == "401_error_page")
                        servers[server - 1].set_401_page(end_str, i);
                    else if (str == "413_error_page")
                        servers[server - 1].set_413_page(end_str, i);
                    else if (str == "501_error_page")
                        servers[server - 1].set_501_page(end_str, i);
                    else if (str == "505_error_page")
                        servers[server - 1].set_505_page(end_str, i);
                    else if (str == "204_error_page")
                        servers[server - 1].set_204_page(end_str, i);
                    else if (str == "412_error_page")
                        servers[server - 1].set_412_page(end_str, i);
                    else if (str == "405_error_page")
                        servers[server - 1].set_405_page(end_str, i);
                    else if (str == "500_error_page")
                        servers[server - 1].set_500_page(end_str, i);
                    else if (str == "502_error_page")
                        servers[server - 1].set_502_page(end_str, i);

                    else if (str == "root"){
                        servers[server - 1].set_root(end_str, i);
                    }
                    else if (str == "index")
                        servers[server - 1].set_index_home(end_str, i);
                    else if (str == "- server")
                        server++;
                    else if (str == "locations"){
                        while (std::getline(input, buffur) && !input.eof()){
                            if (!buffur.empty()){ 
                                i = skip_space_tap(buffur);
                                pos = buffur.find_first_of(':');
                                std::string str = buffur.substr(i, pos - i);
                                f = skip_space_end(buffur);
                                if (str != "- server")
                                    end_str = buffur.substr(pos + 2, f - (pos+1));
                                if (skip_space_tap(end_str) == (int)end_str.size())
                                    throw std::range_error("config file not valide please follow rules");
                                if (str == "- path"){
                                    location++;
                                    servers[server - 1].set_location_atripute(location ,8, end_str, i);
                                }
                                else if (str == "root")
                                    servers[server - 1].set_location_atripute(location ,1, end_str, i);
                                else if (str == "autoindex")
                                    servers[server - 1].set_location_atripute(location ,2, end_str, i);
                                else if (str == "allow_methods")
                                    servers[server - 1].set_location_atripute(location ,3, end_str, i);
                                else if (str == "index")
                                    servers[server - 1].set_location_atripute(location ,4, end_str, i);
                                else if (str == "return")
                                    servers[server - 1].set_location_atripute(location ,5, end_str, i);
                                else if (str == "cgi")
                                    servers[server - 1].set_location_atripute(location ,7, end_str, i);
                                else if (str == "cgi-ext")
                                    servers[server - 1].set_location_atripute(location ,6, end_str, i);
                                else if (str == "upload_pass")
                                    servers[server - 1].set_location_atripute(location ,9, end_str, i);
                                else if (str == "upload_pass_args")
                                    servers[server - 1].set_location_atripute(location ,10, end_str, i);

                                else if (str == "- server"){
                                    location = 0;
                                    server++;
                                    break;
                                }
                                else if (str[0] != '#')
                                    throw std::range_error("check your config file location part;");
                            }
                        }
                    }
                    else if (str[0] != '#')
                        throw std::range_error("check your config file server part;");
                }
            }
        }
    }
}

int Webserve::skip_space_tap(std::string str) {
    for (int i = 0; str[i]; i++){
        if (str[i] != ' ')
            return(i);
        if (str[i] == '\t'){
            throw std::range_error("there is invalid server name?");
        }
    }
    return (0);
}

void Webserve::fill_content_type(){
    c_type[".css"]  = "text/css";
    c_type[".csv"]  = "text/csv";
    c_type[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    c_type[".gif"]  = "image/gif";
    c_type[".htm"]  = "text/html";
    c_type[".html"] = "text/html";
    c_type[".jpeg"] = "image/jpeg";
    c_type[".jpg"]  = "image/jpeg";
    c_type[".js"]   = "text/javascript";
    c_type[".json"] = "application/json";
    c_type[".mp3"]  = "audio/mpeg";
    c_type[".mp4"]  = "video/mp4";
    c_type[".mpeg"] = "video/mpeg";
    c_type[".png"]  = "image/png";
    c_type[".pdf"]  = "application/pdf";
    c_type[".ppt"]  = "application/vnd.ms-powerpoint";
    c_type[".svg"]  = "image/svg+xml";
    c_type[".txt"]  = "text/plain";
    c_type[".bin"]  = "application/octet-stream";
    c_type[".csh"]  = "application/x-csh"; //c shell script
    c_type[".sh"]   = "application/x-sh";
    c_type[".odt"]  = "application/vnd.oasis.opendocument.text";
    c_type[".mjs"]  = "text/javascript";
    c_type[".jar"]  = "application/java-archive";
    c_type[".ico"]  = "image/vnd.microsoft.icon";
    c_type[".pyc"]  = "application/x-python-code";
    c_type[".xml"]  = "application/xml";
    c_type[".php"]  = "application/x-httpd-php";
    c_type[".py"]  = "application/x-python-code";
    c_type[".py"]  = "text/x-python";
}

void Webserve::names_file(){
    n_file["text/css"] = "text_css";
    n_file["text/csv"] = "text_csv";
    n_file["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = "dock_file_docx";
    n_file["image/gif"] = "image_gif";
    n_file["text/html"] = "text_html";
    n_file["text/htm"] = "text_htm";
    n_file["image/jpeg"] = "image_jpeg";
    n_file["text/javascript"] = "text_js";
    n_file["application/json"] = "application_json";
    n_file["audio/mpeg"] = "audio_mp3";
    n_file["video/mp4"] = "video_mp4";
    n_file["video/mpeg"] = "video_mpeg";
    n_file["image/png"] = "image_png";
    n_file["application/pdf"] = "application_pdf";
    n_file["application/vnd.ms-powerpoint"] = "application_vnd_ms_ppt";
    n_file["image/svg+xml"] = "image_svg";
    n_file["text/plain"] = "text_txt";
    n_file["application/octet-stream"] = "application_octet_bin";
    n_file["application/x-csh"] = "application_x_csh";
    n_file["application/x-sh"] = "application_x_sh";
    n_file["application/vnd.oasis.opendocument.text"] = "application_vnd_odt";
    n_file["text/javascript"] = "text_mjs";
    n_file["application/java-archive"] = "application_jar";
    n_file["image/vnd.microsoft.icon"] = "image_vnd_microsoft_ico";
    n_file["application/x-python-code"] = "application_x_py";
    n_file["application/xml"] = "application_xml";
    n_file["application/x-httpd-php"] = "application_x_php";
    n_file["text/x-python"] = "textpython_py";
    n_file[""] = "hamid_txt";
}

void Webserve::fill_vector_name_id(){
    std::vector<Server> serv = getServers();
    for (size_t i = 0; i < nbr_of_serv; i++)
        for (size_t d = 0; d < serv[i].get_location_nbr(); d++){
            if (serv[i].get_from_location_class("path", d).empty())
                serv[i]._locations[d].set_root(serv[i].get_root(), 8);
        }
    for (size_t i = 0; i < nbr_of_serv; i++){
        server_id_osama.push_back(servers[i].get_port());
        server_n_osama.push_back(servers[i].get_server_name());
        if (servers[i].get_host() == "localhost")
            server_host_osama.push_back("127.0.0.1");
        else
            server_host_osama.push_back(servers[i].get_host());
        size_t pos = servers[i].get_server_name().find(' ');
        if (pos != std::string::npos)
            throw std::range_error("there is invalid server name?");
    }

    for (size_t i = 0; i < nbr_of_serv; i++){
        for (size_t d = i + 1; d < nbr_of_serv; d++){
            if (server_n_osama[i] == server_n_osama[d])
                throw std::range_error("there is two servers with same name?");
        }
    }
    sort(server_id_osama.begin(), server_id_osama.end()); 
    size_t h = nbr_of_serv;
    for (size_t i = 0; i < h; i++){
        if (server_id_osama[i] == server_id_osama[i + 1]){
            server_id_osama.erase(server_id_osama.begin() + i);
            h--;
            i--;
        }
    }
}

std::map<std::string, std::string> Webserve::return_location(std::string pathh, int i, int port_nbr){
    std::vector<Server> serv = getServers();
    int nbr = -1;
    int f = -1;
    (void)port_nbr;
    std::string pat = "";

    if (pathh[pathh.size() - 1] != '/' && pathh.find_last_of('.') == std::string::npos)
        pathh += "/";
    for (size_t d = 0; d < serv[i].get_location_nbr(); d++){
        std::string loca = serv[i].get_from_location_class("path", d);
        if (loca == "/")
            f = d;
        if (loca[loca.size() - 1] != '/')
            loca += "/";
        if (pathh[0] != '/')
            pat = "/";
        pat += pathh;
        if (pat.find(loca) == 0 || (pat.find(loca) == 1 && loca[0] == '/')){
            nbr = d;
            break;
        }        
    }
    std::map<std::string, std::string> name;
    if (nbr == -1 && f == -1){
        name["Error"] = "no location matching";
        return (name);
    }
    else if(nbr == -1)
        nbr = f;
    name["path"] = serv[i].get_from_location_class("path", nbr);
    name["return"] = serv[i].get_from_location_class("return_index", nbr);
    string cp_path = (pathh[pathh.size() - 1] != '/') ? pathh + "/" : pathh;
    if (cp_path == name["path"]) {
        name["index"] = serv[i].get_from_location_class("index_home", nbr);
        if (!name["index"].empty())
            name["rtn index"] = "true";
    }
    if (!name["return"].empty()){
        return (name);
    }
    name["root"] = serv[i].get_from_location_class("root", nbr);
    name["autoindex"] = serv[i].get_from_location_class("autoindex", nbr);
    name["allow_methods"] = serv[i].get_from_location_class("allow_methods", nbr);
    name["cgi"] = serv[i].get_from_location_class("cgi", nbr);
    name["cgi-ext"] = serv[i].get_from_location_class("cgi_ext", nbr);
    name["upload_pass"] = serv[i].get_from_location_class("upload_pass", nbr);
    name["upload_pass_args"] = serv[i].get_from_location_class("upload_pass_args", nbr);
    return (name);
}

std::map<std::string, std::string> Webserve::map_location(std::string path, int port, std::string host){
    std::vector<int> nbr_server;
    int nbr = -1;
    int i = -1;
    std::vector<Server> ser = getServers();
    for (size_t d = 0; d < nbr_of_serv; d++){
        if (atoi(ser[d].get_port().c_str()) == port){
            nbr_server.push_back(d);
        }
    }
    if (host.find_last_of(':') != std::string::npos){
        if (atoi(host.substr(host.find_last_of(':') + 1, host.size()).c_str()) == port){
            for(size_t i = 0; i < nbr_server.size(); i++){
                if (ser[nbr_server[i]].get_host() == host){
                    i = nbr_server[i];
                    break;
                }
            }
        }
        else{
            for(size_t i = 0; i < nbr_server.size(); i++){
                if (ser[nbr_server[i]].get_server_name() == host){
                    i = nbr_server[i];
                    break;
                }
            }
        }
    }
    if (i == -1){
        for(size_t i = 0; i < nbr_server.size(); i++){
            if (ser[nbr_server[i]].get_server_name() == "/"){
                i = nbr_server[i];
            }
            if (ser[nbr_server[i]].get_server_name() == host){
                nbr = nbr_server[i];
                break;
            }
        }
        if (nbr == -1 && i == -1)
            return (return_location(path, nbr_server[0], port));
        if (nbr == -1)
            return (return_location(path, i, port));
    }
    return (return_location(path, nbr, port));
}

std::map<std::string, std::string> Webserve::map_location(std::string pathh, int port_nbr){
    int i = 0;
    std::vector<Server> ser = getServers();
    for (size_t d = 0; d < nbr_of_serv; d++){
        if (atoi(ser[d].get_port().c_str()) == port_nbr){
            i = d;
            break;
        }
    }
    return (return_location(pathh, i, port_nbr));
}

std::string Webserve::from_location(std::string path, int port_nbr, std::string u_need){
    int i = 0;
    std::vector<Server> ser = getServers();
    for (size_t d = 0; d < nbr_of_serv; d++){
        if (atoi(ser[d].get_port().c_str()) == port_nbr){
            i = d;
            break;
        }
    }
    int nbr = -1;
    std::string new_path;
    std::vector<Server> serv = getServers();
    if (path[0] != '/')
        new_path = "/";
    size_t last = path.find_last_of('/');
    if (last != std::string::npos)
        new_path += path.substr(0, last + 1);
    else
        new_path += path;
    if (path[path.size() - 1] != '/')
        new_path += "/";
    
    for (size_t d = 0; d < serv[i].get_location_nbr(); d++){
        std::string location = serv[i].get_from_location_class("path", d);
        if (location[0] != '/'){
            location = "/";
            location += serv[i].get_from_location_class("path", d);
        }
        if (location[location.size() - 1] != '/')
            location += "/";
        if (new_path == location){
            nbr = d;
            break;
        }
    }
    return (serv[i].get_from_location_class(u_need, nbr));
}

int Webserve::find_server_using_port(int port){
      std::vector<Server> ser = getServers();
    for (size_t d = 0; d < nbr_of_serv; d++){
        if (atoi(ser[d].get_port().c_str()) == port){
            return (d);
        }
    }
    return (0);
}

int Webserve::find_server_using_port_host(int port, std::string host){
    std::vector<int> nbr_server;
    int nbr = -1;
    int i = -1;
    std::vector<Server> ser = getServers();
    for (size_t d = 0; d < nbr_of_serv; d++){
        if (atoi(ser[d].get_port().c_str()) == port){
            nbr_server.push_back(d);
        }
    }
    if (host.find_last_of(':') != std::string::npos){
        if (atoi(host.substr(host.find_last_of(':') + 1, host.size()).c_str()) == port){
            for(size_t t = 0; t < nbr_server.size(); t++){
                if (ser[nbr_server[t]].get_host() == host){
                    i = nbr_server[t];
                    break;
                }
            }
        }
        else{
            for(size_t t = 0; t < nbr_server.size(); t++){
                if (ser[nbr_server[t]].get_server_name() == host){
                    i = nbr_server[t];
                    break;
                }
            }
        }
    }
    if (i == -1){
        for(size_t t = 0; t < nbr_server.size(); t++){
            if (ser[nbr_server[t]].get_server_name() == "/"){
                i = nbr_server[t];
            }
            if (ser[nbr_server[t]].get_server_name() == host){
                nbr = nbr_server[t];
                break;
            }

        }
        if (nbr == -1 && i == -1){
            return (nbr_server[0]);
        }
        if (nbr == -1)
            return  (i);
    }
    return (nbr);
}
