local ffi = require 'ffi'

local ffi_new = ffi.new
local ffi_cast = ffi.cast
local ffi_sizeof = ffi.sizeof
local tonumber = tonumber
local fmt = string.format

ffi.cdef [[
    typedef uint8_t     sre_char;
    typedef intptr_t    sre_int_t;
    typedef uintptr_t   sre_uint_t;

    /* status code */
    enum {
        SRE_OK       = 0,
        SRE_ERROR    = -1,
        SRE_AGAIN    = -2,
        SRE_BUSY     = -3,
        SRE_DONE     = -4,
        SRE_DECLINED = -5
    };

    /* the memory pool API */
    struct sre_pool_s;
    typedef struct sre_pool_s  sre_pool_t;

    sre_pool_t *sre_create_pool(size_t size);
    void sre_reset_pool(sre_pool_t *pool);
    void sre_destroy_pool(sre_pool_t *pool);

    /* the regex parser API */

    /* regex flags */
    enum {
        SRE_REGEX_CASELESS = 1
    };

    struct sre_regex_s;
    typedef struct sre_regex_s  sre_regex_t;

    sre_regex_t *sre_regex_parse(sre_pool_t *pool, sre_char *src,
        sre_uint_t *ncaps, int flags, sre_int_t *err_offset);
    void sre_regex_dump(sre_regex_t *re);
    sre_regex_t * sre_regex_parse_multi(sre_pool_t *pool,
        sre_char **regexes, sre_int_t nregexes, sre_uint_t *max_ncaps,
        int *multi_flags, sre_int_t *err_offset, sre_int_t *err_regex_id);

    /* the regex compiler API */

    struct sre_program_s;
    typedef struct sre_program_s  sre_program_t;

    void sre_program_dump(sre_program_t *prog);
    sre_program_t *sre_regex_compile(sre_pool_t *pool, sre_regex_t *re);


    /* the Pike VM API */
    struct sre_vm_pike_ctx_s;
    typedef struct sre_vm_pike_ctx_s  sre_vm_pike_ctx_t;

    sre_vm_pike_ctx_t *sre_vm_pike_create_ctx(sre_pool_t *pool,
        sre_program_t *prog, sre_int_t *ovector, size_t ovecsize);

    sre_int_t sre_vm_pike_exec(sre_vm_pike_ctx_t *ctx, sre_char *input,
        size_t len, unsigned eof, sre_int_t **pending_matched);


    /* the Thompson VM API */
    struct sre_vm_thompson_ctx_s;
    typedef struct sre_vm_thompson_ctx_s  sre_vm_thompson_ctx_t;

    sre_vm_thompson_ctx_t *sre_vm_thompson_create_ctx(sre_pool_t *pool,
        sre_program_t *prog);

    sre_int_t sre_vm_thompson_exec(sre_vm_thompson_ctx_t *ctx, sre_char *input,
        size_t len, unsigned eof);


    /* Thompson VM JIT API */
    struct sre_vm_thompson_code_s;
    typedef struct sre_vm_thompson_code_s  sre_vm_thompson_code_t;

    typedef sre_int_t (*sre_vm_thompson_exec_pt)(sre_vm_thompson_ctx_t *ctx,
        sre_char *input, size_t size, unsigned eof);
    
    sre_int_t sre_vm_thompson_jit_compile(sre_pool_t *pool,
        sre_program_t *prog, sre_vm_thompson_code_t **pcode);
    
    sre_vm_thompson_ctx_t *sre_vm_thompson_jit_create_ctx(sre_pool_t *pool,
        sre_program_t *prog);
    
    sre_vm_thompson_exec_pt
        sre_vm_thompson_jit_get_handler(sre_vm_thompson_code_t *code);
    
    sre_int_t sre_vm_thompson_jit_free(sre_vm_thompson_code_t *code);
]]

local sregex = ffi.load('libsregex')

local _M = {
    _VERSION = '0.1.0',
    _SR_VER = '0.1.0',
}

function _M.create_pool(size)
    return sregex.sre_create_pool(size)
end

function _M.destory_pool(pool)
    sregex.sre_destroy_pool(pool)
end

function _M.reset_pool(pool)
    sregex.sre_reset_pool(pool)
end

function _M.regex_parse(pool, pattern, flags)
    local pat = ffi_new('sre_char[?]', #pattern, pattern)
    local ncaps = ffi_new('sre_uint_t[1]')
    local err_offset = ffi.new('sre_int_t[1]')
    local reg = sregex.sre_regex_parse(pool, pat, ncaps, flags, err_offset)
    if reg == nil then
        return nil, nil, fmt('pat "%s" error at %d',
                             pattern, tonumber(err_offset[0]))
    end
    return reg, tonumber(ncaps[0]), nil
end

function _M.regex_parse_multi(pool, patterns, multi_flags)
    local regs = ffi_new('const char*[?]', #patterns + 1, patterns)
    local regs = ffi_cast('sre_char**', regs)
    local ncaps = ffi_new('sre_uint_t[1]')
    local flags = ffi_new('int[?]', #multi_flags, multi_flags)
    local err_offset = ffi.new('sre_int_t[1]')
    local err_reg_id = ffi.new('sre_int_t[1]')
    local r = sregex.sre_regex_parse_multi(pool, regs, #patterns, ncaps,
                                           flags, err_offset, err_reg_id)
    if r == nil then
        return nil, nil, fmt('regexes[%d] error at %d',
                             tonumber(err_reg_id[0]), tonumber(err_offset[0]))
    end
    return r, tonumber(ncaps[0]), nil
end

function _M.regex_dump(reg)
    return sregex.sre_regex_dump(reg)
end

function _M.regex_compile(pool, reg)
    return sregex.sre_regex_compile(pool, reg)
end

function _M.program_dump(prog)
    return sregex.sre_program_dump(prog)
end

--pike
function _M.vm_pike_create_ctx(pool, prog, ncaps)
    local ovecsize = 2 * (ncaps + 1)
    local ovector = ffi_new('sre_int_t[?]', ovecsize)
    local ctx = sregex.sre_vm_pike_create_ctx(pool, prog, ovector, ovecsize)
    if ctx == nil then
        return nil, 'fail to create pike_ctx'
    end
    return ctx, ovector, ovecsize
end

function _M.vm_pike_exec(ctx, data, mode)
    local input = ffi_new('sre_char[?]', #data, data)
    local len = ffi_sizeof(input)
    local pending_matched = ffi_new('sre_int_t*[1]')
    local ret = sregex.sre_vm_pike_exec(ctx, input, len, mode, pending_matched)
    local done = false
    if ret == sregex.SRE_OK then
        done = false
        return true, done, pending_matched
    end
    if ret == sregex.SRE_AGAIN then
        done = true
        return true, done, pending_matched
    end
    return false
end

--thompson
function _M.vm_thompson_create_ctx(pool, prog)
    return sregex.sre_vm_thompson_create_ctx(pool, prog)
end

function _M.vm_thompson_exec(ctx, data, mode)
    local input = ffi_new('sre_char[?]', #data, data)
    local len = ffi_sizeof(input)
    local ret = sregex.sre_vm_thompson_exec(ctx, input, len, mode)
    local done = false
    if ret == sregex.SRE_OK then
        done = false
        return true, done
    end
    if ret == sregex.SRE_AGAIN then
        done = true
        return true, done
    end
    return false
end

function _M.vm_thompson_jit_compile(pool, prog)
    local code = ffi_new('sre_vm_thompson_code_t*[1]')
    local r = sregex.sre_vm_thompson_jit_compile(pool, prog, code)
    if r == sregex.SRE_OK then
        return code[0], nil
    elseif r == sregex.SRE_DECLINED then
        return nil, 'arch not supported'
    elseif r == sregex.SRE_ERROR then
        return nil, 'fatal error'
    end
    return nil, 'unknown error'
end

function _M.vm_thompson_jit_get_handler(code)
    return sregex.sre_vm_thompson_jit_get_handler(code)
end

function _M.vm_thompson_jit_create_ctx(pool, prog)
    return sregex.sre_vm_thompson_jit_create_ctx(pool, prog)
end

function _M.vm_thompson_jit_exec(handler, ctx, data, mode)
    local input = ffi_new('sre_char[?]', #data, data)
    local len = ffi_sizeof(input)
    return handler(ctx, input, len, mode)
end

function _M.vm_thompson_jit_free(code)
    return sregex.sre_vm_thompson_jit_free(code)
end

return _M

