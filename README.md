STAMINA.lib
===========

Stamina.LIB is a helper library I've wrote while working on [Konnekt](http://www.konnekt.info) project. It has quite a lot of helpful classes and the last time it was used, it was fairly stable.

Bear in mind, that it's more than 6 years old, and not maintained anymore.

It's main dependency is Boost library (1.32).

## Interesting stuff

### Stamina::String

String class which transparently handles both 8bit and 16bit unicode strings, doing conversions only when necessary. It's highly optimized and tested. Until you modify the string, it's not copied, so it's quite memory efficient also...


# Projekt KONNEKT

Szczegóły do źródeł znajdziecie w [głównym repozytorium](https://github.com/Konnekt/konnekt), oraz w [grupie Konnekt](https://github.com/Konnekt).

A pozostałe informacje tradycyjnie na [www.konnekt.info](http://www.konnekt.info).

## Licencja

Kod udostępniony jest na licencji Mozilla (MPL). Jej treść znajdziecie w pliku [LICENSE.txt](LICENSE.txt)

## Disclaimer

This is a legacy project published mainly for amusement and for self maintenance for those, that are still using the Konnekt IM.
It's terribly outdated, and was never designed with open source in mind.

