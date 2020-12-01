// Statistics object for a single feature. An analysis can create multiple
// statistics (e.g. for different kinds of template), that's fine.
template<typename ...Types>
class Stats final {
public:
    Stats() : Data() {
    }
    Stats(Types... in) : Data() {
        this->gather(in...);
    }
    std::string Name;
    std::string Description;
    void gather(Types... in){
        this->Data.emplace_back(std::make_tuple(in...));
    }
    void writeToFile(llvm::raw_fd_ostream&& stream){
        for(auto tup : Data) {
            // https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple
            std::apply([&](auto&&... args) {
                ((stream << args << " "), ...);}, tup);
            stream << "\n";
        }
    }
private:
    // Data points
    std::vector<std::tuple<Types...>> Data;
};
