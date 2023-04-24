#ifndef LABWORK_7_ANDREWRYZHIKOV_MP3_PARSER_H
#define LABWORK_7_ANDREWRYZHIKOV_MP3_PARSER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

const static std::unordered_set<std::string> frame_ban_list = {"AENC", "APIC", "ASPI", "GEOB", "MCDI", "LINK",
                                                               "MLLT", "MLLT", "RVRB", "SIGN", "SYTC", "TFLT", "TMED"};

uint32_t TranslationFromSyncSafe(const std::string &sync_safe);

uint64_t TranslationFromBytes64(const std::vector<uint8_t> &bytes, uint32_t size);

uint32_t TranslationFromBytes(const uint8_t* bytes, uint32_t size);

class Header {
public:
    void ParserHeader(std::ifstream &file);

    const static uint8_t size_header = 10, number_bits_flag = 8;

private:
    std::string identifier_ = "ID3";
    std::string version_;
    std::unordered_map<std::string, bool> flags_;
    uint32_t size_header_;
};

class Frame {
public:
    Frame(const std::string &id, const uint32_t &size_frame, const std::unordered_map<std::string, bool> &status_flags,
          const std::unordered_map<std::string, bool> &format_flags) {
        id_ = id;
        size_frame_ = size_frame;
        status_flags_ = status_flags;
        format_flags_ = format_flags;
    }

    std::string getId() {
        return id_;
    }

    const static uint8_t size_header = 10;

    virtual void ParseInformation(std::ifstream &file);

    virtual std::string getInformation(const std::string &info);

    virtual void setInformation(const std::string &info, const std::string &new_info);

    virtual std::string ShowField();

protected:
    std::string id_;
    uint32_t size_frame_;
    std::unordered_map<std::string, bool> status_flags_;
    std::unordered_map<std::string, bool> format_flags_;
};

Frame *ParserFrame(std::ifstream &file);

class TextFrame : public Frame {
public:
    TextFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    std::string ShowField() override;

    void ParseInformation(std::ifstream &file) override;

    std::string getInformation(const std::string &info) override;

    void setInformation(const std::string &info, const std::string &new_info) override;

private:
    uint8_t text_encoding_;
    std::string information_;
};

class TxxxFrame : public Frame {
public:
    TxxxFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    std::string ShowField() override;

    void ParseInformation(std::ifstream &file) override;

    std::string getInformation(const std::string &info) override;

    void setInformation(const std::string &info, const std::string &new_info) override;

private:
    uint8_t text_encoding_;
    std::string description_;
    std::string value_;
};

class UrlFrame : public Frame {
public:
    UrlFrame(const std::string &id, const uint32_t &size_frame,
             const std::unordered_map<std::string, bool> &status_flags,
             const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                format_flags) {}

    std::string ShowField() override;

    void ParseInformation(std::ifstream &file) override;

    std::string getInformation(const std::string &info) override;

    void setInformation(const std::string &info, const std::string &new_info) override;

private:
    std::string information_;
};

class CommFrame : public Frame {
public:
    CommFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}


    std::string ShowField() override;

    void ParseInformation(std::ifstream &file) override;

    std::string getInformation(const std::string &info) override;

    void setInformation(const std::string &info, const std::string &new_info) override;

private:
    uint8_t text_encoding_;
    std::string language_;
    std::string description_;
    std::string information_;
};

class PopFrame : public Frame {
public:
    PopFrame(const std::string &id, const uint32_t &size_frame,
             const std::unordered_map<std::string, bool> &status_flags,
             const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                format_flags) {}


    void ParseInformation(std::ifstream &file) override;

    std::string ShowField() override;

    std::string getInformation(const std::string &info) override;

    void setInformation(const std::string &info, const std::string &new_info) override;

private:
    std::string email_;
    std::string rating_song_;
    uint64_t counter_ = 0;
};

class ComrFrame : public Frame {
public:
    ComrFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}


    void ParseInformation(std::ifstream &file) override;

    std::string ShowField() override;

    void setInformation(const std::string &info, const std::string &new_info) override;

    std::string getInformation(const std::string &info) override;

    const static int size_date = 8;

private:
    uint8_t text_encoding_;
    std::string price_;
    std::string valid_date_;
    std::string url_;
    uint8_t received_;
    std::string name_seller_;
    std::string description_;
    std::string mime_type_;
    std::vector<uint8_t> logo_seller_;
};

class UsltFrame : public Frame {
public:
    UsltFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

    std::string ShowField() override;

    void setInformation(const std::string &info, const std::string &new_info) override;

    std::string getInformation(const std::string &info) override;

private:
    std::string text_encoding_;
    std::string language_;
    std::string description_;
    std::string information_;
};

class EncrFrame : public Frame {
public:
    EncrFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

    std::string ShowField() override;

    std::string getInformation(const std::string &info) override;

    void setInformation(const std::string &info, const std::string &new_info) override;


private:
    std::string identifier_;
    uint8_t method_symbol_;
    std::vector<uint8_t> encryption_data_;
};

class EtcoFrame : public Frame {
public:
    EtcoFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    uint8_t time_stamp_format_;
    std::vector<std::pair<uint8_t, uint32_t>> stamp_and_event;
};

class UfidFrame : public Frame {
public:
    UfidFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

    std::string ShowField() override;

    std::string getInformation(const std::string &info) override;

    void setInformation(const std::string &info, const std::string &new_info) override;

private:
    std::string owner_identifier_;
    std::vector<uint8_t> identifier_;
};

class Eque2Frame : public Frame {
public:
    Eque2Frame(const std::string &id, const uint32_t &size_frame,
               const std::unordered_map<std::string, bool> &status_flags,
               const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                  format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    uint8_t interpolation_method;
    std::string identification;
    std::vector<std::pair<uint16_t, uint16_t>> freq_volum;
};

class McdiFrame : public Frame {
public:
    McdiFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    std::vector<uint8_t> cd_token_;
};

class Channel {
private:
    uint8_t type_;
    std::string volume_adjustment_;
    uint8_t bits_;
    std::string volume_;
public:
    Channel(uint8_t type, std::string volume_adjustment, uint8_t bits, std::string volume) {
        type_ = type;
        volume_adjustment_ = std::move(volume_adjustment);
        bits_ = bits;
        volume_ = std::move(volume);
    }
};

class Rva2Frame : public Frame {
public:
    Rva2Frame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    ~Rva2Frame() {
        for (auto channel: channels) {
            delete channel;
        }
    }

    void ParseInformation(std::ifstream &file) override;

private:
    std::string identification_;
    std::vector<Channel *> channels;
};

class RbufFrame : public Frame {
public:
    RbufFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    uint32_t size_buffer_;
    bool flag_embedded_;
    uint32_t offset_;
};

class OwneFrame : public Frame {
public:
    OwneFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

    std::string ShowField() override;

    std::string getInformation(const std::string &info) override;

    void setInformation(const std::string &info, const std::string &new_info) override;

private:
    uint8_t text_encoring_;
    std::string price_;
    std::string date_;
    std::string seller_;
};

class PossFrame : public Frame {
public:
    PossFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    uint8_t format_time_;
    uint64_t position_;
};

class UserFrame : public Frame {
public:
    UserFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

    std::string ShowField() override;

    std::string getInformation(const std::string &info) override;

    void setInformation(const std::string &info, const std::string &new_info) override;

private:
    uint8_t text_encoring_;
    std::string language_;
    std::string actual_text_;
};

class PcntFrame : public Frame {
public:
    PcntFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    uint64_t counter_;
};

class GridFrame : public Frame {
public:
    GridFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    std::string ownre_identifier_;
    uint8_t group_symbol_;
    std::vector<uint8_t> group_data_;
};

class PrivFrame : public Frame {
public:
    PrivFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    std::string own_identifier_;
    std::vector<uint8_t> private_data_;
};

class SeekFrame : public Frame {
public:
    SeekFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    uint32_t minimum_offset_next_tag_;
};

class SyltFrame : public Frame {
public:
    SyltFrame(const std::string &id, const uint32_t &size_frame,
              const std::unordered_map<std::string, bool> &status_flags,
              const std::unordered_map<std::string, bool> &format_flags) : Frame(id, size_frame, status_flags,
                                                                                 format_flags) {}

    void ParseInformation(std::ifstream &file) override;

private:
    uint8_t text_encoding_;
    std::string language_;
    uint8_t time_stamp_;
    uint8_t content_type_;
    std::string descriptor_;
    std::vector<std::pair<std::string, uint64_t>> identifier_time_stamp;
};

class ID3 {
public:
    explicit ID3(const std::string &file_path) {
        path_ = file_path;
    }

    ~ID3() {
        for (auto frame: frames_) {
            delete frame;
        }
    }

    void Parser();

    Frame *FindFrame(const std::string &id);

private:
    std::vector<Frame *> frames_;
    std::string path_;
    Header header_;
};


#endif //LABWORK_7_ANDREWRYZHIKOV_MP3_PARSER_H
