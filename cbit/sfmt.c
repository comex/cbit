str str_sfmt_impl(const char *fmt, ...) {
    str outstr = VEC_INIT_BARE_STATIC(&outstr, char);
    va_list ap;
    va_start(ap, fmt);
    while (1) {
        char c = *fmt++;
        if (c == 0)
            break;
        if (c != '%') {
            vec_append_ccp(&outstr, c);
            continue;
        }
        /*
            1. %
            2. [#-+ 0']*
            3. vec separator character (OS X ext, ignore)
            4. -?[0-9]*|\*([0-9]+\$)  field width (ignore explicit position)
            5. (\.<same-as-field-width>)?  precision
            6. length (ignore)
            7. format char
        */
        char 
        switch (argtype) {
        case SFAT_END:
            printf("end\n");
            return;
        #define X(ty, e) case e: va_arg(ap, ty); printf("%s\n", #e); break
        X(         int,       SFAT_B);
        X(         int,       SFAT_UC);
        X(         int,       SFAT_SC);
        X(         int,       SFAT_C);
        X(         int,       SFAT_US);
        X(         int,       SFAT_S);
        X(unsigned int,       SFAT_UI);
        X(         int,       SFAT_I);
        X(unsigned long,      SFAT_UL);k
        X(         long,      SFAT_L);
        X(unsigned long long, SFAT_ULL);
        X(         long long, SFAT_LL);
        X(         double,    SFAT_F);
        X(         double,    SFAT_D);
        X(         char *,    SFAT_CSTR);
        default:
            printf("?? %d\n", argtype);
            abort();
        }
    }
    {
        int argtype = va_arg(ap, int);
        if (argtype != SFAT_END)
            cbit_panic("str_sfmt_impl: wrong number of arguments with format string <%s>", fmt);
    }
    va_end(ap);
}


int main() {
    str_sfmt("fmt", 2, 2ul, (short)2000, "lol", 42.0, 42.0f, (char)'f');
}
