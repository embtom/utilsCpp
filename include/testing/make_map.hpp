namespace detail {
template <class T, class Tuple, std::size_t... I>
constexpr T make_from_tuple_impl( Tuple&& t, std::index_sequence<I...> )
{
  return T(std::get<I>(std::forward<Tuple>(t))...);
}
} // namespace detail

template <class T, class Tuple>
constexpr T make_from_tuple( Tuple&& t )
{
    return detail::make_from_tuple_impl<T>(std::forward<Tuple>(t),
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}


// template <typename... Args0, typename... Args1>
// void f(std::tuple<Args0...> args0, std::tuple<Args1...> args1) {
//    auto alpha = make_from_tuple<Object0>(args0);
//    auto beta = make_from_tuple<Object1>(args1);
// }


template <typename A, typename B, typename... Args>
std::unordered_map<std::remove_reference_t<A>,std::remove_reference_t<B>> make_map(Args&&... args)
{
    std::unordered_map<std::remove_reference_t<A>,std::remove_reference_t<B>> map;
    map.reserve(sizeof...(Args));
    using arr_t = int[];
    std::cout << sizeof...(Args) << std::endl;
    (void) arr_t{0, (map.emplace(std::forward<Args>(std::pair<A,B>(std::move(args)))),0)...}; // std::piecewise_construct,
//    (void) arr_t{0, (map.emplace(std::forward_as_tuple<Args>(make_from_tuple<std::unordered_map<A,B>>(args))),0)...};
    return map;
}

template <class A, class B>
std::pair<std::remove_reference_t<A>, std::remove_reference_t<B>> create_pair(A&& first) {
    return std::make_pair(std::forward<A>(first), std::forward<B>(first));
}


    //std::pair pair = std::make_pair(LogLevel::Control, LogLevel::Critical);
    // std::unordered_map<LogLevel, CharStream> testMap2 = make_map<LogLevel, CharStream>(std::pair<LogLevel, CharStream>(LogLevel::Critical, LogLevel::Critical));
    
    // //auto it = testMap2.find(LogLevel::Critical);

    // //(it->second) << "test" << std::endl;

    // std::unordered_map<LogLevel, CharStream> testMap  = make_map<LogLevel, CharStream>(create_pair<LogLevel, CharStream>(LogLevel::Critical));
    // auto it = testMap.find(LogLevel::Critical);
    // if (it != testMap.end()) {
    //     CharStream &str = it->second;
    //      str<<"test" << std::endl;
    // }
    // //testMap[LogLevel::Control];

    //  CharStream &str2 = m_logStreamBufs[0];
    //   str2 << "aber"<< std::endl;
   // testMap[LogLevel::Critical] << "hallo" << std::endl;
    // make_map<LogLevel, LogLevel>(pair2);
    


    //testMap.reserve(3);
    //testMap.emplace(pair);
    // //std::map<LogLevel, LogLevel> testMap = {{LogLevel::Control, LogLevel::Error}};



    // std::ostream os (&m_logStreamBufs[0]);
    // os << "halddfdflo" << std::endl;

    // CEtLogStream<char> test (LogLevel::Error);
    // test << "baldlf" << std::endl;

    // CEtLogStream<char> test2 (std::move(test));
    // test << "badfdfdfdfdflf" << std::endl;
