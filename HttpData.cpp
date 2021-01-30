#include "HttpData.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include "Channel.h"
#include "EventLoop.h"
#include "Util.h"
#include "time.h"

std::unordered_map<std::string, std::string> MimeType::mime;
pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;

const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;
const int DEFAULT_EXPIRED_TIME = 2000;                     //ms
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000;         //ms

char favicon[555] = {
    '\x89', 'P',    'N',    'G',    '\xD',  '\xA',  '\x1A', '\xA',  '\x0',
    '\x0',  '\x0',  '\xD',  'I',    'H',    'D',    'R',    '\x0',  '\x0',
    '\x0',  '\x10', '\x0',  '\x0',  '\x0',  '\x10', '\x8',  '\x6',  '\x0',
    '\x0',  '\x0',  '\x1F', '\xF3', '\xFF', 'a',    '\x0',  '\x0',  '\x0',
    '\x19', 't',    'E',    'X',    't',    'S',    'o',    'f',    't',
    'w',    'a',    'r',    'e',    '\x0',  'A',    'd',    'o',    'b',
    'e',    '\x20', 'I',    'm',    'a',    'g',    'e',    'R',    'e',
    'a',    'd',    'y',    'q',    '\xC9', 'e',    '\x3C', '\x0',  '\x0',
    '\x1',  '\xCD', 'I',    'D',    'A',    'T',    'x',    '\xDA', '\x94',
    '\x93', '9',    'H',    '\x3',  'A',    '\x14', '\x86', '\xFF', '\x5D',
    'b',    '\xA7', '\x4',  'R',    '\xC4', 'm',    '\x22', '\x1E', '\xA0',
    'F',    '\x24', '\x8',  '\x16', '\x16', 'v',    '\xA',  '6',    '\xBA',
    'J',    '\x9A', '\x80', '\x8',  'A',    '\xB4', 'q',    '\x85', 'X',
    '\x89', 'G',    '\xB0', 'I',    '\xA9', 'Q',    '\x24', '\xCD', '\xA6',
    '\x8',  '\xA4', 'H',    'c',    '\x91', 'B',    '\xB',  '\xAF', 'V',
    '\xC1', 'F',    '\xB4', '\x15', '\xCF', '\x22', 'X',    '\x98', '\xB',
    'T',    'H',    '\x8A', 'd',    '\x93', '\x8D', '\xFB', 'F',    'g',
    '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f',    'v',    'f',    '\xDF',
    '\x7C', '\xEF', '\xE7', 'g',    'F',    '\xA8', '\xD5', 'j',    'H',
    '\x24', '\x12', '\x2A', '\x0',  '\x5',  '\xBF', 'G',    '\xD4', '\xEF',
    '\xF7', '\x2F', '6',    '\xEC', '\x12', '\x20', '\x1E', '\x8F', '\xD7',
    '\xAA', '\xD5', '\xEA', '\xAF', 'I',    '5',    'F',    '\xAA', 'T',
    '\x5F', '\x9F', '\x22', 'A',    '\x2A', '\x95', '\xA',  '\x83', '\xE5',
    'r',    '9',    'd',    '\xB3', 'Y',    '\x96', '\x99', 'L',    '\x6',
    '\xE9', 't',    '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',    '\xA7',
    '\xC4', 'b',    '1',    '\xB5', '\x5E', '\x0',  '\x3',  'h',    '\x9A',
    '\xC6', '\x16', '\x82', '\x20', 'X',    'R',    '\x14', 'E',    '6',
    'S',    '\x94', '\xCB', 'e',    'x',    '\xBD', '\x5E', '\xAA', 'U',
    'T',    '\x23', 'L',    '\xC0', '\xE0', '\xE2', '\xC1', '\x8F', '\x0',
    '\x9E', '\xBC', '\x9',  'A',    '\x7C', '\x3E', '\x1F', '\x83', 'D',
    '\x22', '\x11', '\xD5', 'T',    '\x40', '\x3F', '8',    '\x80', 'w',
    '\xE5', '3',    '\x7',  '\xB8', '\x5C', '\x2E', 'H',    '\x92', '\x4',
    '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g',    '\x98', '\xE9',
    '6',    '\x1A', '\xA6', 'g',    '\x15', '\x4',  '\xE3', '\xD7', '\xC8',
    '\xBD', '\x15', '\xE1', 'i',    '\xB7', 'C',    '\xAB', '\xEA', 'x',
    '\x2F', 'j',    'X',    '\x92', '\xBB', '\x18', '\x20', '\x9F', '\xCF',
    '3',    '\xC3', '\xB8', '\xE9', 'N',    '\xA7', '\xD3', 'l',    'J',
    '\x0',  'i',    '6',    '\x7C', '\x8E', '\xE1', '\xFE', 'V',    '\x84',
    '\xE7', '\x3C', '\x9F', 'r',    '\x2B', '\x3A', 'B',    '\x7B', '7',
    'f',    'w',    '\xAE', '\x8E', '\xE',  '\xF3', '\xBD', 'R',    '\xA9',
    'd',    '\x2',  'B',    '\xAF', '\x85', '2',    'f',    'F',    '\xBA',
    '\xC',  '\xD9', '\x9F', '\x1D', '\x9A', 'l',    '\x22', '\xE6', '\xC7',
    '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15', '\x90', '\x7',  '\x93',
    '\xA2', '\x28', '\xA0', 'S',    'j',    '\xB1', '\xB8', '\xDF', '\x29',
    '5',    'C',    '\xE',  '\x3F', 'X',    '\xFC', '\x98', '\xDA', 'y',
    'j',    'P',    '\x40', '\x0',  '\x87', '\xAE', '\x1B', '\x17', 'B',
    '\xB4', '\x3A', '\x3F', '\xBE', 'y',    '\xC7', '\xA',  '\x26', '\xB6',
    '\xEE', '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
    '\xA',  '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X',    '\x0',  '\x27',
    '\xEB', 'n',    'V',    'p',    '\xBC', '\xD6', '\xCB', '\xD6', 'G',
    '\xAB', '\x3D', 'l',    '\x7D', '\xB8', '\xD2', '\xDD', '\xA0', '\x60',
    '\x83', '\xBA', '\xEF', '\x5F', '\xA4', '\xEA', '\xCC', '\x2',  'N',
    '\xAE', '\x5E', 'p',    '\x1A', '\xEC', '\xB3', '\x40', '9',    '\xAC',
    '\xFE', '\xF2', '\x91', '\x89', 'g',    '\x91', '\x85', '\x21', '\xA8',
    '\x87', '\xB7', 'X',    '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N',
    'N',    'b',    't',    '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
    '\xEC', '\x86', '\x2',  'H',    '\x26', '\x93', '\xD0', 'u',    '\x1D',
    '\x7F', '\x9',  '2',    '\x95', '\xBF', '\x1F', '\xDB', '\xD7', 'c',
    '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF', '\x22', 'J',    '\xC3',
    '\x87', '\x0',  '\x3',  '\x0',  'K',    '\xBB', '\xF8', '\xD6', '\x2A',
    'v',    '\x98', 'I',    '\x0',  '\x0',  '\x0',  '\x0',  'I',    'E',
    'N',    'D',    '\xAE', 'B',    '\x60', '\x82'
};

void MimeType::init() {
  mime[".html"] = "text/html";
  mime[".avi"] = "video/x-msvideo";
  mime[".bmp"] = "image/bmp";
  mime[".c"] = "text/plain";
  mime[".doc"] = "application/msword";
  mime[".gif"] = "image/gif";
  mime[".gz"] = "application/x-gzip";
  mime[".htm"] = "text/html";
  mime[".ico"] = "image/x-icon";
  mime[".jpg"] = "image/jpeg";
  mime[".png"] = "image/png";
  mime[".txt"] = "text/plain";
  mime[".mp3"] = "audio/mp3";
  mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string &suffix) {
    if (mime.find(suffix) == mime.end())
        return mime["default"];
    else 
        return mime[suffix];
}


HttpData::HttpData(EventLoop *loop, int connfd)
    : loop_(loop),
    fd_(connfd),
    channel_(new Channel(loop, connfd)),
    error_(false),
    connectionState_(H_CONNECTED),
    method_(METHOD_GET),
    HTTPVersion_(HTTP_11),
    nowReadPos_(0),
    state_(STATE_PARSE_URI),
    hState_(H_START),
    keepAlive_(false) {
    channel_->setReadHandler(bind(&HttpData::handleRead, this));
    channel_->setWriteHandler(bind(&HttpData::handleWrite, this));
    channel_->setConnHanler(bind(&HttpData::handleConn, this));
}

void HttpData::handleRead() {
    __uint32_t &ev = channel_->getEvents(); // not clear
    
    do {
        bool zero = false;
        int num = readn(fd_, inBuffer_, zero);
        // LOG << "Request: " << inBuffer_;
        if (connectionState_ == H_DISCONNECTING) {
            inBuffer_.clear();
            break;
        }

        if (num < 0) {
            perror("Http::handleRead read bytes < 0");
            error_ = true;
            handleError(fd_, 400, "Bad Request");
            break;
        }

        if (zero) {
            connectionState_ = H_DISCONNECTING;
            if (num == 0)
                break;
        }

        // parse the request line
        if (state_ = STATE_PARSE_URI) {
            URIState flag = parseURI();
            if (flag == PARSE_URI_AGAIN)
                break;
            else if (flag == PARSE_URI_ERROR) {
                perror("HttpData::handleRead parseURI error");
                LOG << "FD = " << fd_ << "," << inBuffer_ << "*****";
                inBuffer_.clear();
                error_ = true;
                handleError(fd_, 400, "Bad Request");
                break;
            }
            else
                state_ = STATE_PARSE_HEADERS;
        }

        if (state_ == STATE_PARSE_HEADERS) {
            HeaderState  flag = parseHeaders();
            if (flag == PARSE_HEADER_AGAIN)
                break;
            else if (flag == PARSE_HEADER_ERROR) {
                error_ = true;
                inBuffer_.clear();
                perror("HttpData::handleRead parseHeaders error");
                handleError(fd_, 400, "Bad Request");
                break;
            }
            
            if (method_ == METHOD_POST) 
                state_ = STATE_RECV_BODAY;
            else
                state_ = STATE_ANALYSIS;
        }

        // to make sure the body length in the post request
        if (state_ == STATE_RECV_BODAY) {
            int content_length = - 1;
            if (headers_.find("Content-Length") == headers_.end()) {
                error_ = true;
                handleError(fd_, 400, "Bad Request");
                break;
            }
            content_length = stoi(headers_["Content-Length"]);
            if (static_cast<int>(inBuffer_.size()) < content_length)
                break;
            state_ = STATE_ANALYSIS;
        }

        if (state_ == STATE_ANALYSIS) {
            AnalysisState flag = analysisRequest();
            if (flag == ANALYSIS_SUCCESS) {
                state_ = STATE_FINISH;
                break;
            }
            error_ = true;
            break;
        }
    } while(false);

    if (!error_) {
        // first to reply peer
        if (outBuffer_.size() > 0) {
            handleWrite();
        }

        // not clear this section used for what ?
        if (!error_ && state_ == STATE_FINISH) {
            this->reset();

            if (connectionState_ != H_DISCONNECTING && inBuffer_.size() > 0)
                handleRead();
        }

        else if (!error_ && connectionState_ != H_DISCONNECTED)
            ev |= EPOLLIN;
    }
}

void HttpData::handleWrite() {
    __uint32_t &ev = channel_->getEvents();
    if (!error_ && connectionState_ != H_DISCONNECTED) {
        int num_write = writen(fd_, outBuffer_);
        if (num_write < 0) {
            error_ = true;
            ev = 0;
            perror("HttpData::handleWrite write error");
        }

        if (outBuffer_.size() > 0)
            ev |= EPOLLOUT;
    }
}

URIState HttpData::parseURI() {
    std::string &str = inBuffer_;
    
    size_t pos = str.find('\r', nowReadPos_);
    if (pos < 0)
        return PARSE_URI_AGAIN;
    
    std::string request_line = str.substr(0, pos);
    if (str.size() > pos + 1)
        str = str.substr(pos + 1);
    else
        str.clear();
    std::cout<<str[0]<<std::endl;

    //size_t length = request_line.size();
    int posGet = request_line.find("GET");
    int posPost = request_line.find("POST");
    int posHead = request_line.find("HEAD");
    // parse method
    if (posGet >= 0) {
        pos = posGet;
        method_ = METHOD_GET;
    }
    else if (posPost >= 0) {
        pos = posPost;
        method_ = METHOD_POST;
    }
    else if (posHead >= 0) {
        pos = posHead;
        method_ = METHOD_HEAD;
    }
    else
        return PARSE_URI_ERROR;
    // parse filename
    pos = request_line.find('/', pos);
    // in this case means only a GET in request line
    if (pos < 0) {
        fileName_ = "index.html";
        HTTPVersion_ = HTTP_11;
        return PARSE_URI_SUCCESS;
    }
    // normal case
    // GET /fileName HTTP/1.1\r\n
    else {
        // _pos means second blank which after the filename.
        size_t _pos = request_line.find(' ', pos);
        if (_pos < 0)
            return PARSE_URI_ERROR;
        if (_pos - pos > 1) {
            fileName_ = request_line.substr(pos + 1, _pos - pos - 1);
            size_t __pos = fileName_.find('?');
            if (__pos > 0)
                fileName_ = fileName_.substr(0, __pos);
        }
        else
            fileName_ = "index.html";
        pos = _pos;
    }
    // parse httpversion
    // GET /fileName HTTP/1.1(\r\n) pos is the position after the filename.
    pos = request_line.find("/", pos);
    if (pos < 0)
        return PARSE_URI_ERROR;
    else {
        if (request_line.size() - pos < 4)
            return PARSE_URI_ERROR;
        std::string ver = request_line.substr(pos + 1, 3);
        if (ver == "1.1")
            HTTPVersion_ = HTTP_11;
        else if (ver == "1.0")
            HTTPVersion_ = HTTP_10;
        else
            return PARSE_URI_ERROR;
    }
    return PARSE_URI_SUCCESS;
}

HeaderState HttpData::parseHeaders() {
    std::string &str = inBuffer_;
    int key_begin = -1, key_end = -1, value_begin = -1, value_end = -1;
    int last_read_position = -1;
    bool not_finish = true;

    for (size_t i = 0; i < str.size() && !not_finish; i++) {
        switch (hState_)
        {
        case H_START: {
            if (str[i] == '\r' || str[i] == '\n')
                break;
            key_begin = i;
            last_read_position = i;
            hState_ = H_KEY;
            break;
        }
        case H_KEY: {
            if (str[i] == ':') {
                key_end = i;
                if (key_end - key_begin <= 0)
                    return PARSE_HEADER_ERROR;
                hState_ = H_COLON;
            }
            else if (str[i] == '\r' || str[i] == '\n')
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_COLON: {
            if (str[i] == ' ') {
                hState_ = H_SPACES_AFTER_COLON;
            }
            else
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_SPACES_AFTER_COLON: {
            hState_ = H_VALUE;
            value_begin = i;
            break;
        }
        case H_VALUE: {
            if (str[i] == '\r') {
                hState_ = H_CR;
                value_end = i;
                if (value_end - value_begin <= 0)
                    return PARSE_HEADER_ERROR;
            }
            else if (i - value_begin > 255)
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_CR: {
            if (str[i] == '\n') {
                hState_ = H_LF;
                std::string key(str.begin() + key_begin, str.begin() + key_end);
                std::string value(str.begin() + value_begin, str.begin() + value_end);
                headers_[key] = value;
                last_read_position = i;
            }
            else 
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_LF: {
            if (str[i] == '\r') {
                hState_ = H_END_CR;
            }
            else {
                hState_ = H_KEY; // another turn
                key_begin = i;
            }
            break;
        }
        case H_END_CR: {
            if (str[i] == '\n') {
                hState_ = H_END_LF;
            }
            else
                return PARSE_HEADER_ERROR;
            break;
        }
        case H_END_LF: {
            not_finish = false;
            //key_begin = i;
            last_read_position = i;
            break;
        }
        }
    }
    if (hState_ == H_END_LF) {
            str = str.substr(i);
            return PARSE_HEADER_SUCCESS;
        }
    // in case recv data is not complete
    str = str.substr(last_read_position);
    return PARSE_HEADER_AGAIN;
}

