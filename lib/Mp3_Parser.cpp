#include "Mp3_Parser.h"
#include "fstream"
#include "bitset"
#include "vector"

uint32_t TranslationFromSyncSafe(const std::string &sync_safe) {
    uint32_t size = 0;
    uint32_t deegre_two = 1;
    for (int i = 3; i >= 0; --i) {
        for (int z = 0; z < 7; ++z) {
            if ((((1 << z) & (sync_safe[i])) >> z)) {
                size += deegre_two;
            }
            deegre_two *= 2;
        }
    }
    return size;
}

uint32_t TranslationFromBytes(const uint8_t* bytes, uint32_t size) {
    uint32_t counter = 0;
    for (uint32_t i = 0; i < size; ++i) {
        counter = counter | (bytes[size - i - 1] << (8 * i));
    }
    return counter;
}

uint64_t TranslationFromBytes64(const std::vector<uint8_t> &bytes, uint32_t size) {
    uint64_t counter = 0;
    for (uint32_t i = 0; i < size; ++i) {
        counter = counter | (bytes[size - i - 1] << (8 * i));
    }
    return counter;
}

void Header::ParserHeader(std::ifstream &file) {
    char *buff = new char[Header::size_header];
    file.read(buff, Header::size_header);

    version_ += buff[3];
    version_ += buff[4];

    char flag = buff[5];
    std::bitset<Header::number_bits_flag> bits_flag(flag);
    flags_["desynchronization"] = bits_flag[7];
    flags_["extended_header"] = bits_flag[6];
    flags_["experimental indicator"] = bits_flag[5];
    flags_["footer"] = bits_flag[4];

    std::string sync_syfe;
    for (int i = 6; i <= 9; ++i) {
        sync_syfe += buff[i];
    }
    size_header_ = TranslationFromSyncSafe(sync_syfe);

    delete[] buff;
}

void ID3::Parser() {
    std::ifstream file;
    file.open(path_, std::ios::binary);
    header_.ParserHeader(file);

    while (true) {
        auto frame = ParserFrame(file);
        if (frame == nullptr) {
            break;
        } else {
            frames_.push_back(frame);
        }
    }

    file.seekg(-Frame::size_header, std::ios::cur);
}

Frame *ID3::FindFrame(const std::string &id) {
    for (auto &frame: frames_) {
        if (frame->getId() == id) {
            return frame;
        }
    }
    throw std::invalid_argument("This frame don't exist");
}

Frame *ParserFrame(std::ifstream &file) {

    while (true) {

        char *buff = new char[Frame::size_header];
        file.read(buff, Header::size_header);

        std::string id;
        for (int i = 0; i < 4; ++i) {
            id += buff[i];
        }

        uint32_t size_frame = 0;
        std::string sync_syfe;
        for (int i = 4; i < 8; ++i) {
            sync_syfe += buff[i];
        }

        size_frame = TranslationFromSyncSafe(sync_syfe);

        std::unordered_map<std::string, bool> status_flags;
        std::unordered_map<std::string, bool> format_flags;

        for (int i = 8; i < 10; ++i) {
            if (i == 8) {
                char flag_status = buff[8];
                std::bitset<Header::number_bits_flag> bits_flag(flag_status);
                status_flags["change_tag"] = bits_flag[6];
                status_flags["change_file"] = bits_flag[5];
                status_flags["only_reading"] = bits_flag[4];
            } else {
                char flag_status = buff[9];
                std::bitset<Header::number_bits_flag> bits_flag(flag_status);
                format_flags["group identity"] = bits_flag[6];
                format_flags["compression"] = bits_flag[3];
                format_flags["encryption"] = bits_flag[2];
                format_flags["desynchronization"] = bits_flag[1];
                format_flags["length indicator"] = bits_flag[0];
            }
        }
        delete[] buff;

        if (frame_ban_list.count(id) != 0) {
            file.seekg(size_frame, std::ios::cur);
            continue;
        }

        for (int i = 0; i < 4; ++i) {
            if (!(('A' <= id[i] and id[i] <= 'Z') or ('0' <= id[i] and id[i] <= '9'))) {
                file.seekg(-size_frame, std::ios::cur);
                return nullptr;
            }
        }

        Frame *new_frame = nullptr;
        if (id[0] == 'T' and id != "TXXX") {
            new_frame = new TextFrame(id, size_frame, status_flags, format_flags);
        } else if (id[0] == 'W') {
            new_frame = new UrlFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "TXXX") {
            new_frame = new TxxxFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "COMM") {
            new_frame = new CommFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "UFID") {
            new_frame = new UfidFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "RBUF") {
            new_frame = new RbufFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "MCDI") {
            new_frame = new McdiFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "RVA2") {
            new_frame = new Rva2Frame(id, size_frame, status_flags, format_flags);
        } else if (id == "POPM") {
            new_frame = new PopFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "USLT") {
            new_frame = new UsltFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "COMR") {
            new_frame = new ComrFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "ENCR") {
            new_frame = new EncrFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "ETCO") {
            new_frame = new EtcoFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "EQU2") {
            new_frame = new Eque2Frame(id, size_frame, status_flags, format_flags);
        } else if (id == "POSS") {
            new_frame = new PossFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "OWNE") {
            new_frame = new OwneFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "GRID") {
            new_frame = new GridFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "PCNT") {
            new_frame = new PcntFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "USER") {
            new_frame = new UserFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "PRIV") {
            new_frame = new PrivFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "SEEK") {
            new_frame = new SeekFrame(id, size_frame, status_flags, format_flags);
        } else if (id == "SYLT") {
            new_frame = new SyltFrame(id, size_frame, status_flags, format_flags);
        }

        new_frame->ParseInformation(file);
        return new_frame;
    }
}

void Frame::ParseInformation(std::ifstream &file) {}

std::string Frame::getInformation(const std::string &info) {}

void Frame::setInformation(const std::string &info, const std::string &new_info) {}

std::string Frame::ShowField() {}

void TextFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;

    buff = file.get();
    text_encoding_ = buff;
    bytes_without_main_info++;

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; i++) {
        buff = file.get();
        information_ += static_cast<char>(buff);
    }
}

std::string TextFrame::getInformation(const std::string &info) {
    if (info == "information") {
        return information_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

void TextFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "information") {
        information_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string TextFrame::ShowField() {
    return "Available fields: information";
}

void UrlFrame::ParseInformation(std::ifstream &file) {
    uint8_t buff;
    for (uint32_t i = 0; i < size_frame_; i++) {
        buff = file.get();
        information_ += static_cast<char>(buff);
    }
}

std::string UrlFrame::getInformation(const std::string &info) {
    if (info == "information") {
        return information_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

void UrlFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "information") {
        information_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string UrlFrame::ShowField() {
    return "Available fields: information";
}

void CommFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;

    uint8_t buff;
    buff = file.get();
    bytes_without_main_info++;
    text_encoding_ = static_cast<char>(buff);

    for (int i = 0; i < 3; ++i) {
        buff = file.get();
        language_ += static_cast<char>(buff);
        bytes_without_main_info++;
    }

    buff = file.get();
    bytes_without_main_info++;

    while (buff != 0) {
        description_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        information_ += static_cast<char>(buff);
    }
}

std::string CommFrame::getInformation(const std::string &info) {
    if (info == "language") {
        return language_;
    } else if (info == "description") {
        return description_;
    } else if (info == "information") {
        return information_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

void CommFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "language") {
        language_ = new_info;
    } else if (info == "description") {
        description_ = new_info;
    } else if (info == "information") {
        information_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string CommFrame::ShowField() {
    return "Available fields: language, description, information";
}

void PopFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;

    uint8_t buff;
    buff = file.get();
    bytes_without_main_info++;

    while (buff != 0) {
        email_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    buff = file.get();
    bytes_without_main_info++;
    rating_song_ = static_cast<char>(buff);

    std::vector<uint8_t> counter(size_frame_ - bytes_without_main_info);
    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        counter[i] = buff;
    }

    counter_ = TranslationFromBytes64(counter, counter.size());
}

std::string PopFrame::getInformation(const std::string &info) {
    if (info == "rating_song") {
        return email_;
    } else if (info == "rating_song") {
        return rating_song_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

void PopFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "email") {
        email_ = new_info;
    } else if (info == "rating_song") {
        rating_song_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string PopFrame::ShowField() {
    return "Available fields: email, rating_song";
}

void UsltFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;

    uint8_t text_encoding;
    text_encoding = file.get();
    bytes_without_main_info += 1;
    text_encoding_ += static_cast<char>(text_encoding);

    uint8_t language;
    for (int i = 0; i < 3; ++i) {
        language = file.get();
        language_ = static_cast<char>(language);
        bytes_without_main_info++;
    }

    uint8_t buff;
    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        description_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        information_ += static_cast<char>(buff);
    }
}

void UsltFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "language") {
        language_ = new_info;
    } else if (info == "description") {
        description_ = new_info;
    } else if (info == "information") {
        information_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string UsltFrame::getInformation(const std::string &info) {
    if (info == "language") {
        return language_;
    } else if (info == "description") {
        return description_;
    } else if (info == "information") {
        return information_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string UsltFrame::ShowField() {
    return "Available fields: language, description, information";
}

void ComrFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;

    uint8_t buff;
    buff = file.get();
    bytes_without_main_info++;
    text_encoding_ += buff;

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        price_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    for (int i = 0; i < size_date; i++) {
        buff = file.get();
        valid_date_ += static_cast<char>(buff);
        bytes_without_main_info++;
    }

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        url_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    buff = file.get();
    bytes_without_main_info++;
    received_ = buff;

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        name_seller_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        description_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        mime_type_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    for (uint32_t i = 0; i < size_date - bytes_without_main_info; ++i) {
        buff = file.get();
        logo_seller_.push_back(buff);
    }
}

std::string ComrFrame::getInformation(const std::string &info) {
    if (info == "price") {
        return price_;
    } else if (info == "valid_date") {
        return valid_date_;
    } else if (info == "url") {
        return url_;
    } else if (info == "name_seller") {
        return name_seller_;
    } else if (info == "description") {
        return description_;
    } else if (info == "mime_type") {
        return mime_type_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

void ComrFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "price") {
        price_ = new_info;
    } else if (info == "valid_date") {
        valid_date_ = new_info;
    } else if (info == "url") {
        url_ = new_info;
    } else if (info == "name_seller") {
        name_seller_ = new_info;
    } else if (info == "description") {
        description_ = new_info;
    } else if (info == "mime_type") {
        mime_type_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string ComrFrame::ShowField() {
    return "Available fields: price, valid_date, url, name_seller, description, mime_type";
}

void EncrFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;

    buff = file.get();
    bytes_without_main_info++;

    while (buff != 0) {
        identifier_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    buff = file.get();
    bytes_without_main_info++;
    method_symbol_ = buff;

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        encryption_data_.push_back(buff);
    }
}

void EncrFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "identifier") {
        identifier_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string EncrFrame::getInformation(const std::string &info) {
    if (info == "identifier") {
        return identifier_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string EncrFrame::ShowField() {
    return "Available fields: identifier";
}


void EtcoFrame::ParseInformation(std::ifstream &file) {
    uint8_t buff;

    buff = file.get();
    time_stamp_format_ = buff;

    for (uint32_t i = 0; i < (size_frame_ - 1) / 5; ++i) {
        uint8_t type_event;
        type_event = file.get();

        auto *stamp = new uint8_t[4];
        for (int j = 0; j < 4; j++) {
            stamp[j] = file.get();
        }

        stamp_and_event.emplace_back(type_event, TranslationFromBytes(stamp, 4));
    }
}

void UfidFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;
    buff = file.get();

    while (buff != 0) {
        owner_identifier_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        identifier_.push_back(buff);
    }
}

std::string UfidFrame::getInformation(const std::string &info) {
    if (info == "owner_identifier") {
        return owner_identifier_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

void UfidFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "owner_identifier") {
        owner_identifier_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string UfidFrame::ShowField() {
    return "Available fields: owner_identifier";
}

void Eque2Frame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;
    buff = file.get();
    bytes_without_main_info++;
    interpolation_method = buff;

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        identification += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    for (uint64_t i = 0; i < (size_frame_ - bytes_without_main_info) / 4; ++i) {
        uint8_t f_byte;
        uint8_t s_byte;

        f_byte = file.get();
        s_byte = file.get();
        uint16_t freq = (f_byte << 8) | s_byte;

        f_byte = file.get();
        s_byte = file.get();
        uint16_t volum = (f_byte << 8) | s_byte;

        freq_volum.emplace_back(freq, volum);
    }
}

void McdiFrame::ParseInformation(std::ifstream &file) {
    uint8_t buff;
    for (uint32_t i = 0; i < size_frame_; ++i) {
        buff = file.get();
        cd_token_.push_back(buff);
    }
}

void Rva2Frame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;
    while (bytes_without_main_info < size_frame_) {
        buff = file.get();
        bytes_without_main_info++;
        while (buff != 0) {
            identification_ += static_cast<char>(buff);
            buff = file.get();
            bytes_without_main_info++;
        }

        for (uint32_t i = 0; i < (size_frame_ - bytes_without_main_info) / 4; i++) {
            uint32_t bytes_without_main_info = 0;

            buff = file.get();
            bytes_without_main_info++;
            uint8_t type = buff;

            std::string adjustment;
            for (int i = 0; i < 2; ++i) {
                buff = file.get();
                bytes_without_main_info++;
                adjustment += static_cast<char>(buff);
            }

            buff = file.get();
            bytes_without_main_info++;
            uint8_t bits = buff;

            std::string volume;
            for (int i = 0; i < bits; ++i) {
                buff = file.get();
                bytes_without_main_info++;
                volume += static_cast<char>(buff);
            }
            Channel *new_channel = new Channel(type, adjustment, bits, volume);
            channels.push_back(new_channel);
        }
    }
}

void RbufFrame::ParseInformation(std::ifstream &file) {
    uint8_t buff;

    uint8_t *size = new uint8_t(3);
    for (int i = 0; i < 3; ++i) {
        buff = file.get();
        size[i] = buff;
    }
    size_buffer_ = TranslationFromBytes(size, 3);

    buff = file.get();
    flag_embedded_ = std::bitset<8>(buff)[0];

    uint8_t *offset = new uint8_t(4);
    for (int i = 0; i < 4; ++i) {
        buff = file.get();
        offset[i] = buff;
    }
    offset_ = TranslationFromBytes(offset, 4);
}

void OwneFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;

    buff = file.get();
    bytes_without_main_info++;
    text_encoring_ = buff;

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        price_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    for (int i = 0; i < 8; i++) {
        buff = file.get();
        bytes_without_main_info++;
        date_ += static_cast<char>(buff);
    }

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        date_ += static_cast<char>(buff);
    }
}

std::string OwneFrame::getInformation(const std::string &info) {
    if (info == "price") {
        return price_;
    } else if (info == "date") {
        return date_;
    } else if (info == "seller") {
        return seller_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

void OwneFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "price") {
        price_ = new_info;
    } else if (info == "date") {
        date_ = new_info;
    } else if (info == "seller") {
        seller_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string OwneFrame::ShowField() {
    return "Available fields: price, date, seller";
}

void PossFrame::ParseInformation(std::ifstream &file) {
    uint8_t buff;

    buff = file.get();
    format_time_ = buff;

    std::vector<uint8_t> position(size_frame_ - 1);
    for (uint32_t i = 0; i < size_frame_ - 1; ++i) {
        buff = file.get();
        position[i] = buff;
    }

    position_ = TranslationFromBytes64(position, position.size());
}

void UserFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;

    buff = file.get();
    bytes_without_main_info++;
    text_encoring_ = buff;

    for (int i = 0; i < 3; ++i) {
        buff = file.get();
        language_ += static_cast<char>(buff);
        bytes_without_main_info++;
    }

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        actual_text_ += static_cast<char>(buff);
        bytes_without_main_info++;
    }
}

std::string UserFrame::getInformation(const std::string &info) {
    if (info == "language") {
        return language_;
    } else if (info == "actual_text") {
        return actual_text_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

void UserFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "language") {
        language_ = new_info;
    } else if (info == "actual_text") {
        actual_text_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string UserFrame::ShowField() {
    return "Available fields: language, actual_text, seller";
}

void PcntFrame::ParseInformation(std::ifstream &file) {
    uint8_t buff;
    std::vector<uint8_t> counter(size_frame_);

    for (uint32_t i = 0; i < size_frame_; i++) {
        buff = file.get();
        counter[i] = buff;
    }

    counter_ = TranslationFromBytes64(counter, counter.size());
}

void GridFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        ownre_identifier_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    buff = file.get();
    bytes_without_main_info++;
    group_symbol_ = buff;

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        group_data_.push_back(buff);
    }
}

void PrivFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        own_identifier_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        private_data_.push_back(buff);
    }
}

void SeekFrame::ParseInformation(std::ifstream &file) {
    uint8_t buff;

    uint8_t *offset = new uint8_t[4];
    for (int i = 0; i < 4; ++i) {
        buff = file.get();
        offset[i] = buff;
    }

    minimum_offset_next_tag_ = TranslationFromBytes(offset, 4);
}

void SyltFrame::ParseInformation(std::ifstream &file) {
    uint8_t buff;
    uint32_t bytes_without_main_info = 0;

    buff = file.get();
    bytes_without_main_info++;
    text_encoding_ = buff;

    for (int i = 0; i < 3; ++i) {
        buff = file.get();
        bytes_without_main_info++;
        language_ += static_cast<char>(buff);
    }

    buff = file.get();
    bytes_without_main_info++;
    time_stamp_ = buff;

    buff = file.get();
    bytes_without_main_info++;
    content_type_ = buff;

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        descriptor_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    uint8_t separator;
    separator = file.get();
    bytes_without_main_info++;

    while (bytes_without_main_info < size_frame_) {
        std::string identifier;
        std::vector<uint8_t> time_stamp;

        buff = file.get();
        bytes_without_main_info++;

        while (buff != 0 and bytes_without_main_info < size_frame_) {
            identifier += static_cast<char>(buff);
            buff = file.get();
            bytes_without_main_info++;
        }

        buff = file.get();
        bytes_without_main_info++;
        while (buff != separator and bytes_without_main_info < size_frame_) {
            time_stamp.push_back(buff);
            buff = file.get();
            bytes_without_main_info++;
        }
        identifier_time_stamp.emplace_back(identifier, TranslationFromBytes64(time_stamp, time_stamp.size()));
    }

}

void TxxxFrame::ParseInformation(std::ifstream &file) {
    uint32_t bytes_without_main_info = 0;
    uint8_t buff;

    buff = file.get();
    bytes_without_main_info++;
    text_encoding_ = buff;

    buff = file.get();
    bytes_without_main_info++;
    while (buff != 0) {
        description_ += static_cast<char>(buff);
        buff = file.get();
        bytes_without_main_info++;
    }

    for (uint32_t i = 0; i < size_frame_ - bytes_without_main_info; ++i) {
        buff = file.get();
        value_ +=  static_cast<char>(buff);
    }
}

std::string TxxxFrame::getInformation(const std::string &info) {
    if (info == "description") {
        return description_;
    } else if (info == "value") {
        return value_;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

void TxxxFrame::setInformation(const std::string &info, const std::string &new_info) {
    if (info == "description") {
        description_ = new_info;
    } else if (info == "value") {
        value_ = new_info;
    } else {
        throw std::invalid_argument(ShowField());
    }
}

std::string TxxxFrame::ShowField() {
    return "Available fields: description, value";
}
