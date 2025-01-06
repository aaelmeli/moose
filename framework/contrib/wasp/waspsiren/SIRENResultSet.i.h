#ifndef SIREN_SIRENRESULTSET_I_H
#define SIREN_SIRENRESULTSET_I_H

template<typename TAdapter>
SIRENResultSet<TAdapter>::SIRENResultSet()
{
}
template<typename TAdapter>
SIRENResultSet<TAdapter>::SIRENResultSet(const SIRENResultSet<TAdapter>& orig)
    : adapted_results(orig.adapted_results)
    , scalar_results(orig.scalar_results)
    , results(orig.results)
{
}
template<typename TAdapter>
SIRENResultSet<TAdapter>::~SIRENResultSet()
{
}

template<typename TAdapter>
void SIRENResultSet<TAdapter>::push(const TAdapter& node)
{
    results.push_back(
        {ADAPTED, static_cast<unsigned int>(adapted_results.size())});
    adapted_results.push_back(node);
}
template<typename TAdapter>
void SIRENResultSet<TAdapter>::push(const std::string& name,
                                    const std::string& data)
{
    results.push_back(
        {SCALAR, static_cast<unsigned int>(scalar_results.size())});
    scalar_results.push_back({name, data});
}

#endif
