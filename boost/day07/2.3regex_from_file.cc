#include <boost/regex.hpp>
#include <iostream>
#include <fstream>

int main() {
    boost::regex::flag_type flag;
    //flag = boost::regex::perl; 
    flag = boost::regex::perl|boost::regex::icase;
    //flag |= boost::regex::no_except;

    std::string regex = "\\b(?:s(?:e(?:t(?:_(?:e(?:xception|rror)_handler|magic_quotes_runtime|include_path)|defaultstub)|ssion_s(?:et_save_handler|tart))|qlite_(?:(?:(?:unbuffered|single|array)_)?query|create_(?:aggregate|function)|p?open|exec)|tr(?:eam_(?:context_create|socket_client)|ipc?slashes|rev)|implexml_load_(?:string|file)|ocket_c(?:onnect|reate)|h(?:ow_sourc|a1_fil)e|pl_autoload_register|ystem)|p(?:r(?:eg_(?:replace(?:_callback(?:_array)?)?|match(?:_all)?|split)|oc_(?:(?:terminat|clos|nic)e|get_status|open)|int_r)|o(?:six_(?:get(?:(?:e[gu]|g)id|login|pwnam)|mk(?:fifo|nod)|ttyname|kill)|pen)|hp(?:_(?:strip_whitespac|unam)e|version|info)|g_(?:(?:execut|prepar)e|connect|query)|a(?:rse_(?:ini_file|str)|ssthru)|utenv)|r(?:unkit_(?:function_(?:re(?:defin|nam)e|copy|add)|method_(?:re(?:defin|nam)e|copy|add)|constant_(?:redefine|add))|e(?:(?:gister_(?:shutdown|tick)|name)_function|ad(?:(?:gz)?file|_exif_data|dir))|awurl(?:de|en)code)|i(?:mage(?:createfrom(?:(?:jpe|pn)g|x[bp]m|wbmp|gif)|(?:jpe|pn)g|g(?:d2?|if)|2?wbmp|xbm)|s_(?:(?:(?:execut|write?|read)ab|fi)le|dir)|ni_(?:get(?:_all)?|set)|terator_apply|ptcembed)|g(?:et(?:_(?:c(?:urrent_use|fg_va)r|meta_tags)|my(?:[gpu]id|inode)|(?:lastmo|cw)d|imagesize|env)|z(?:(?:(?:defla|wri)t|encod|fil)e|compress|open|read)|lob)|a(?:rray_(?:u(?:intersect(?:_u?assoc)?|diff(?:_u?assoc)?)|intersect_u(?:assoc|key)|diff_u(?:assoc|key)|filter|reduce|map)|ssert(?:_options)?)|h(?:tml(?:specialchars(?:_decode)?|_entity_decode|entities)|(?:ash(?:_(?:update|hmac))?|ighlight)_file|e(?:ader_register_callback|x2bin))|f(?:i(?:le(?:(?:[acm]tim|inod)e|(?:_exist|perm)s|group)?|nfo_open)|tp_(?:nb_(?:ge|pu)|connec|ge|pu)t|(?:unction_exis|pu)ts|write|open)|o(?:b_(?:get_(?:c(?:ontents|lean)|flush)|end_(?:clean|flush)|clean|flush|start)|dbc_(?:result(?:_all)?|exec(?:ute)?|connect)|pendir)|m(?:b_(?:ereg(?:_(?:replace(?:_callback)?|match)|i(?:_replace)?)?|parse_str)|(?:ove_uploaded|d5)_file|ethod_exists|ysql_query|kdir)|e(?:x(?:if_(?:t(?:humbnail|agname)|imagetype|read_data)|ec)|scapeshell(?:arg|cmd)|rror_reporting|val)|c(?:url_(?:file_create|exec|init)|onvert_uuencode|reate_function|hr)|u(?:n(?:serialize|pack)|rl(?:de|en)code|[ak]?sort)|(?:json_(?:de|en)cod|debug_backtrac|tmpfil)e|b(?:(?:son_(?:de|en)|ase64_en)code|zopen)|var_dump)(?:\\s|/\\*.*\\*/|//.*|#.*)*\\(.*\\)";

    std::ifstream file("raw.txt");
    std::string str;
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        str = buffer.str();
        file.close();
    }
    
    const boost::regex e(regex, flag);
    if (e.status()) {
        std::cout << "Incorrect regex pattern!\n";
        return 0;
    }
    
    boost::smatch results;
    bool matched = boost::regex_search(str, results, e);
    //std::cout << "regex: " << regex << std::endl;
    std::cout << (matched ? "MATCH\n" : "DOES NOT MATCH\n");
    if (matched) {
        //std::copy(
        //    results.begin(), 
        //    results.end(), 
        //    std::ostream_iterator<std::string>( std::cout, ", ")
        //);

        std::cout << "size: " << results.size() << std::endl;
        for (size_t i = 0; i < results.size(); i++) {
            std::cout << results[i] << std::endl;
        }
    }
                
    return 0;
}

//g++ 2.2regext_test.cc -L/root/file/boost_1_72_0/stage/lib -lboost_regex 
