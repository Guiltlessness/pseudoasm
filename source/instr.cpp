#include "instr.h"
#include "executor.h"

#include <iostream>
#include <tuple>
#include <vector>
#include <map>

comp::executor vm;
std::string unval_mem_infuture;

namespace  {
template<typename Q>
std::string to_bcode(Q const& s) {
    return std::string((const char*)&s, sizeof (Q));
}

template<typename Q>
Q parse(std::string const& s);

template<>
AllWhatYouWantToKnowAboutInstr::fpos_t parse(std::string const& s) {
    return { AllWhatYouWantToKnowAboutInstr::pos_t(std::stoi(s)) };
}
template<>
AllWhatYouWantToKnowAboutInstr::mpos_t parse(std::string const& s) {
    return { AllWhatYouWantToKnowAboutInstr::pos_t(std::stoi(s)) };
}

template<>
comp::int_t parse<comp::int_t>(std::string const& s) {
    try {
        return std::stoi(s);
    } catch (std::exception& e) {
        throw std::runtime_error(e.what() + std::string(", when the args is: ") + s);
    }
}
template<>
comp::float_t parse<comp::float_t>(std::string const& s) {
    return comp::float_t(std::stod(s));
}
template<>
comp::str_t parse<comp::str_t>(std::string const& s) {
    std::size_t left = 0, right = 0;
    for (                 ; s[left] != '\"' && left < s.size(); ++left);
    for ( right = left + 1; s[right] != '\"' && right < s.size(); ++right);
    if ((right - left) < 2 && s[left] != '\"' && s[right] != '\"')
        throw std::runtime_error("Expected \'\"\'");
    std::size_t ans = unval_mem_infuture.size();
    unval_mem_infuture.append(s.c_str() + left + 1, right - left - 1);
    unval_mem_infuture.push_back('\0');
    return comp::str_t{ int(ans) };
}
template<>
comp::nreg_t parse<comp::nreg_t>(std::string const&s) {
    static const std::map<std::string, comp::nreg_t> reg_map {
        {"iax", 0 },    {"fax", 0 },
        {"ibx", 1 },    {"fbx", 1 },
        {"icx", 2 },    {"fcx", 2 },
        {"idx", 3 },    {"fdx", 3 },
        {"iex", 4 },    {"fex", 4 },
        {"ifx", 5 },    {"ffx", 5 },
        {"igx", 6 },    {"fgx", 6 },
        {"ihx", 7 },    {"fhx", 7 },
        {"iix", 8 },    {"fix", 8 },
        {"ijx", 9 },    {"fjx", 9 },
        {"ikx", 10},    {"fkx", 10},
        {"ilx", 11},    {"flx", 11},
        {"imx", 12},    {"fmx", 12},
        {"inx", 13},    {"fnx", 13},
        {"iox", 14},    {"fox", 14},
        {"ipx", 15},    {"fpx", 15},
        // int reg       float reg
    };
    std::size_t left = 0, right = s.size() - 1;
    for(; s[right] != 'x' && right > left; --right);
    for(; (s[left] != 'i' && s[left] != 'f') && left < right; ++left) {}
    if(left == right)
        throw std::runtime_error("Expected reg");
    return reg_map.at(std::string(s.c_str() + left, right - left + 1));
}

template<typename F, typename ...T, size_t ...N>
void deliver_impl(std::tuple<T...> const& tpl, F f, std::index_sequence<N...>) noexcept {
    f(std::get<N>(tpl)...);
}

template<typename ...T, typename F>
void deliver(std::tuple<T...> const& tpl, F f) noexcept {
    deliver_impl(tpl, f, std::make_index_sequence<sizeof... (T)>());
}

template<typename ...T>
struct instr_impl {
    std::string name;
    std::function<void(T...)> func;
    std::function<void(const char*)> exec;
    std::function<std::string(const char*)> get_byte_args;
    std::tuple<T...> arg_buf;
    std::size_t argssize;

    instr_impl(std::string const& nm, std::function<void(T...)> f)
        :name(nm)
        ,func(f)
    {
        std::vector<std::size_t> sz{sizeof (T)...};
        for(auto i : sz)
            argssize += i;

        exec = [this](const char* code){
            this->load_args(code, std::make_index_sequence<sizeof... (T)>());
            deliver(arg_buf, func);
        };

        get_byte_args = [this](const char* code) {
            return this->parse_args_to_bcode(code);
        };
    }
    template<std::size_t N, typename PN = typename std::tuple_element<N, std::tuple<T...>>::type>
    int load_args_impl(const char* &code) noexcept {
        std::get<N>(arg_buf) = *(const PN*)code;
        code += sizeof(decltype (std::get<N>(arg_buf)));
        return 1;
    }

    template<std::size_t ...N>
    void load_args(__attribute__((unused)) const char* code, std::index_sequence<N...>) noexcept {
        std::vector<int>{load_args_impl<N>(code)...};
    }

    std::vector<std::string> parse_args_to_vec(const char* code) {
        std::vector<std::string> ans;
        std::size_t countargs = sizeof... (T);
        const char* prev = code;
        for (; *code != '\0'; ++code) {
            if(*code == ',') {
                if ((code - prev) > 0)
                    ans.emplace_back(prev, code - prev);
                prev = code + 1;
            }
        }
        if ((code - prev) > 0)
            ans.emplace_back(prev, code - prev);

        if(ans.size() != countargs) {
            throw std::runtime_error("Expected more args\n" + std::to_string(ans.size()) + " vs expected: " + std::to_string(countargs));
        }
        return ans;
    }

    template<std::size_t ...N>
    std::string parse_a_t_bc_impl(const char* code, std::index_sequence<N...>) {
        std::size_t size = 0;
        auto args = parse_args_to_vec(code);
        std::vector<std::string> pre_ans = {to_bcode<typename std::tuple_element<N, std::tuple<T...>>::type>(
                                 parse   <typename std::tuple_element<N, std::tuple<T...>>::type>(args[N]))...};
        for(auto const& i : pre_ans)
            size += i.size();
        std::string ans;
        ans.reserve(size);
        for(auto const& i : pre_ans)
            ans.append(i);
        return ans;
    }
    std::string parse_args_to_bcode(const char* code) {
        return parse_a_t_bc_impl(code, std::make_index_sequence<sizeof... (T)>());
    }

};

class collector_t {
    std::vector< std::function<void()> > cont;
public:
    collector_t() {}
    void push( std::function<void()> && val) noexcept {
        cont.push_back(val);
    }
    ~collector_t() {
        for(auto &i : cont) {
            i();
        }
    }
};

collector_t collector;


template<typename ...T>
AllWhatYouWantToKnowAboutInstr::instr_kind findout_kind() {
    std::vector<bool> fc_findout {(typeid (T) == typeid (AllWhatYouWantToKnowAboutInstr::fpos_t))...};
    std::vector<bool> mk_findout = {(typeid (T) == typeid (AllWhatYouWantToKnowAboutInstr::mpos_t))...};
    bool fcallable = false;
    bool mleaping = false;

    for (size_t i = 0; i < sizeof... (T); ++i) {
        fcallable |= fc_findout[i];
        mleaping |= mk_findout[i];
    }
    if(fcallable && mleaping)
        throw std::runtime_error("Ambigous a mark argument (func_name or mark_name)");

    AllWhatYouWantToKnowAboutInstr::instr_kind kind = AllWhatYouWantToKnowAboutInstr::instr_kind::USUAL;
    if (fcallable)
        kind = AllWhatYouWantToKnowAboutInstr::instr_kind::FUNCCALLABLE;
    if (mleaping)
        kind = AllWhatYouWantToKnowAboutInstr::instr_kind::MARKLEAPING;
    return kind;
}
template<typename ...T, typename Name>
AllWhatYouWantToKnowAboutInstr::instr_t make_instr(Name const& nm, void(*fn)(T...)) {
    std::string name(nm);
    std::function<void(T...)> f = fn;
    auto val = new instr_impl<T...>(name, f);
    collector.push([val](){
        delete val;
    });
    AllWhatYouWantToKnowAboutInstr::instr_kind kind;
    try {
        kind = findout_kind<T...>();
    } catch (std::runtime_error & e) {
        throw std::runtime_error(std::string(e.what()) + " in " + name);
    }
    return AllWhatYouWantToKnowAboutInstr::instr_t (val->name,
                                                    val->exec,
                                                    val->get_byte_args,
                                                    val->argssize,
                                                    kind);
}

void pushi(int i) noexcept {
    comp::executor::type val;
    val.i = i;
    vm.valstack.push( val );
}
void pushf(float f) noexcept {
    comp::executor::type val;
    val.f = f;
    vm.valstack.push( val );
}

void outi() noexcept {
    std::cout << vm.valstack.top().i << " ";
    vm.valstack.pop();
}
void popi(comp::nreg_t nreg) noexcept {
    vm.regi[nreg] = vm.valstack.top().i;
    vm.valstack.pop();
}
void popf(comp::nreg_t nreg) noexcept {
    vm.regf[nreg] = vm.valstack.top().f;
    vm.valstack.pop();
}
void pushir(comp::nreg_t n) noexcept {
    comp::executor::type val;
    val.i = vm.regi[n];
    vm.valstack.push(val);
}
void pushfr(comp::nreg_t n) noexcept {
    comp::executor::type val;
    val.f = vm.regf[n];
    vm.valstack.push(val);
}
void addi() noexcept {
    auto i1 = vm.valstack.top().i; vm.valstack.pop();
    auto i2 = vm.valstack.top().i; vm.valstack.pop();
    comp::executor::type val; val.i = i1 + i2;
    vm.valstack.push(val);
}
void subi() noexcept {
    auto i1 = vm.valstack.top().i; vm.valstack.pop();
    auto i2 = vm.valstack.top().i; vm.valstack.pop();
    comp::executor::type val; val.i = i2 - i1;
    vm.valstack.push(val);
}
void muli() noexcept {
    auto i1 = vm.valstack.top().i; vm.valstack.pop();
    auto i2 = vm.valstack.top().i; vm.valstack.pop();
    comp::executor::type val; val.i = i1 * i2;
    vm.valstack.push(val);
}
void addf() noexcept {
    auto f1 = vm.valstack.top().f; vm.valstack.pop();
    auto f2 = vm.valstack.top().f; vm.valstack.pop();
    comp::executor::type val; val.f = f1 + f2;
    vm.valstack.push(val);
}
void out_endl() noexcept {
    std::cout << std::endl;
}
void ini() noexcept {
comp::int_t i; std::cin >> i;
comp::executor::type val;
val.i = i;
vm.valstack.push(val);
}
void call(AllWhatYouWantToKnowAboutInstr::fpos_t p) noexcept {
    vm.callstack.push(vm.pos);
    vm.pos = p.pos;
}
void ret() noexcept {
    vm.pos = vm.callstack.top();
    vm.callstack.pop();

    if (vm.callstack.size() == 0) {
        vm.is_on = false;
    }
}
void jmp (AllWhatYouWantToKnowAboutInstr::mpos_t p) noexcept {
    vm.pos = p.pos;
}
void je (AllWhatYouWantToKnowAboutInstr::mpos_t p) noexcept {
auto v1 = vm.valstack.top().i; vm.valstack.pop();
auto v2 = vm.valstack.top().i; vm.valstack.pop();
if (v1 == v2)
    vm.pos = p.pos;
}
void outf() noexcept {
    std::cout << vm.valstack.top().f << " ";
    vm.valstack.pop();
}
void inf() noexcept {
    comp::float_t f;
    comp::executor::type val;
    std::cin >> f; val.f = f;
    vm.valstack.push(val);
}
void ext() noexcept {
    vm.is_on = false;
}
void outs(comp::str_t val) noexcept {
    std::cout << (vm.unvarmem + val.pos) << " ";
}

void chee() noexcept {
    std:: cout << "██████████████▀▀▀▀▀▀▀▀▀▀▀▀██████████████\n"
                  "████████████▀░░░░░░░░░░░░░░░░▀██████████\n"
                  "██████████▀░░▄███████▄▄░░░░░░░░▀████████\n"
                  "█████████▀░▄█████████████▄▄░░░░░░▀██████\n"
                  "█████████░░░▀███████████████▄░░░░░░█████\n"
                  "████████▀██▄▄░████████████████░░░░░░████\n"
                  "███████░▀░░▄▀█████▀░░░▀▀██████░░░░░░░███\n"
                  "██████▀▄▄▄░░▄█████▀▀▀██▄░████░░░░░░░░░██\n"
                  "█████▀▄████▀▄█▀██░▀░░▄▀█████░░░░░░░░░░██\n"
                  "█████░████▄███░███▄▄▄▄▄█████░░░░░░░░░░▀█\n"
                  "███████▀░▀█████▀████████████░░░░░░░░░░▄█\n"
                  "██████░▄████▄▄█████████████░░░▄░░░░░░░██\n"
                  "███▀▀█▄█▀██████████████████▄███░░░░░░███\n"
                  "░░░░░░██░▀░▄█▀██░░███████████░▀░░░░░▄███\n"
                  "░░░░░░███▄░░░░▄█▄███████████▀▄░░░░░░░░░▀\n"
                  "░░░░░░████████████████████▄▀░░░░░░░░░░░░\n"
                  "░░░░░░██▀█████████████▀███░░░░░░░░░░░░░░\n"
                  "░░░░░░█████████████▀░▄██▀░░░░░░░░░░░░░░░\n"
                  "░░░░░░░▀▀█████████▄███▀▄▀░░░░░░░░░░░░░░░\n";
}

}
namespace AllWhatYouWantToKnowAboutInstr {

list::list()
{
    instr_t buf[] = {
        make_instr("pushi", pushi),
        make_instr("pushf", pushf),
        make_instr("pushir", pushir),
        make_instr("pushfr", pushfr),
        make_instr("addi", addi),
        make_instr("addf", addf),
        make_instr("subi", subi),
        make_instr("muli", muli),
        make_instr("outi", outi),
        make_instr("popi", popi),
        make_instr("popf", popf),
        make_instr("outf", outf),
        make_instr("oendl", out_endl),
        make_instr("ini", ini),
        make_instr("inf", inf),
        make_instr("call", call),
        make_instr("ret", ret),
        make_instr("jmp", jmp),
        make_instr("je", je),
        make_instr("exit", ext),
        make_instr("outs", outs),
        make_instr("chee", chee)
    };
    count_instrs = sizeof (buf) / sizeof (instr_t);
    instrs = new instr_t[count_instrs];
    for (size_t i = 0; i < count_instrs; ++i) {
        instrs[i] = buf[i];
    }
}
list::~list() {
    delete [] instrs;
}

instr_t* const& list::get_list() const noexcept {
    return instrs;
}

std::size_t list::get_list_size() const noexcept {
    return count_instrs;
}

AllWhatYouWantToKnowAboutInstr::instr_t::~instr_t()
{}

AllWhatYouWantToKnowAboutInstr::instr_t::instr_t()
{}

AllWhatYouWantToKnowAboutInstr::instr_t::instr_t(
        std::string const&s,
        std::function<void(const char*)> const& e,
        std::function<std::string(const char*)> const& g,
        std::size_t const& as,
        instr_kind k)
    : name (s)
    , exec (e)
    , get_bytes_args(g)
    , argssize(as)
    , kind(k)
{}

}

