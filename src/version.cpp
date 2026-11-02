#include "version.hpp"

namespace loupe {

const char*
version_line() {
    return "loupe " LOUPE_VERSION ", autana " LOUPE_AUTANA_COMMIT;
}

} // namespace loupe
