//define class
class Base64
{
    public:
        Base64();
        ~Base64();
        int encode(unsigned char *src, unsigned char *dest);
        int decode(unsigned char *src, unsigned char *dest);
    private:
        char alpha[65];
        int encode_process(unsigned char *src, unsigned char *dest, int pad_num);
        int decode_process(unsigned char *src, unsigned char *dest);
        int alpha_revert(unsigned char src);
};
