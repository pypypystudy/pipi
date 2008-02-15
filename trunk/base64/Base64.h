//define class
class Base64
{
    public:
        Base64();
        ~Base64();
        int encode(char *src, char *dest);
        int decode(char *src, char *dest);
    private:
        char alpha[65];
        int encode_process(char *src, char *dest, int pad_num);
        int decode_process(char *src, char *dest);
        int alpha_revert(char src);
};
