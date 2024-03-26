use strict;
use warnings;

package Modsec2LRW;

use Clone qw(clone);
use List::MoreUtils qw(any none);
use Try::Tiny;
use Exporter::Declare;

exports qw(
    valid_line
    clean_input
    tokenize
    parse_tokens
    parse_vars
    parse_operator
    parse_actions
    strip_encap_quotes
    build_chains
    translate_chains
    translate_chain
    translate_vars
    translate_operator
    translate_actions
    figure_phase
    translate_macro
    @auto_expand_operators
);

export_tag translate => qw(
    clean_input
    tokenize
    parse_tokens
    build_chains
    translate_chains
);

my @valid_directives = qw(SecRule SecAction SecDefaultAction SecMarker);

my $valid_vars = {
    ARGS                    => { type => 'REQUEST_ARGS', parse => [ qw(values 1) ] },
    ARGS_GET                => { type => 'URI_ARGS', parse => [ qw(values 1) ] },
    ARGS_GET_NAMES          => { type => 'URI_ARGS', parse => [ qw(keys 1) ] },
    ARGS_NAMES              => { type => 'REQUEST_ARGS', parse => [ qw(keys 1) ] },
    ARGS_POST               => { type => 'REQUEST_BODY', parse => [ qw(values 1) ] },
    ARGS_POST_NAMES         => { type => 'REQUEST_BODY', parse => [ qw(keys 1) ] },
    MATCHED_VAR             => { type => 'MATCHED_VAR' },
    MATCHED_VARS            => { type => 'MATCHED_VARS' },
    MATCHED_VAR_NAME        => { type => 'MATCHED_VAR_NAME' },
    MATCHED_VAR_NAMES       => { type => 'MATCHED_VAR_NAMES' },
    QUERY_STRING            => { type => 'QUERY_STRING' },
    REMOTE_ADDR             => { type => 'REMOTE_ADDR' },
    REQUEST_BASENAME        => { type => 'REQUEST_BASENAME' },
    REQUEST_BODY            => { type => 'REQUEST_BODY' },
    REQUEST_COOKIES         => { type => 'COOKIES', parse => [ qw(values 1) ] },
    REQUEST_COOKIES_NAMES   => { type => 'COOKIES', parse => [ qw(keys 1) ] },
    REQUEST_FILENAME        => { type => 'URI' },
    REQUEST_HEADERS         => { type => 'REQUEST_HEADERS', parse => [ qw(values 1) ] },
    REQUEST_HEADERS_NAMES   => { type => 'REQUEST_HEADERS', parse => [ qw(keys 1) ] },
    REQUEST_LINE            => { type => 'REQUEST_LINE' },
    REQUEST_METHOD          => { type => 'METHOD' },
    REQUEST_PROTOCOL        => { type => 'PROTOCOL' },
    REQUEST_URI             => { type => 'REQUEST_URI' },
    RESPONSE_BODY           => { type => 'RESPONSE_BODY' },
    RESPONSE_CONTENT_LENGTH => { type => 'RESPONSE_HEADERS', parse => [ qw(specific Content-Length) ] },
    RESPONSE_CONTENT_TYPE   => { type => 'RESPONSE_HEADERS', parse => [ qw(specific Content-Type) ] },
    RESPONSE_HEADERS        => { type => 'RESPONSE_HEADERS', parse => [ qw(values 1) ] },
    RESPONSE_HEADERS_NAMES  => { type => 'RESPONSE_HEADERS', parse => [ qw(keys 1) ] },
    RESPONSE_PROTOCOL       => { type => 'PROTOCOL' },
    RESPONSE_STATUS         => { type => 'STATUS' },
    SERVER_NAME             => { type => 'REQUEST_HEADERS', parse => [ qw(specific Host) ] },
    TIME                    => { type => 'TIME' },
    TIME_DAY                => { type => 'TIME_DAY' },
    TIME_EPOCH              => { type => 'TIME_EPOCH' },
    TIME_HOUR               => { type => 'TIME_HOUR' },
    TIME_MIN                => { type => 'TIME_MIN' },
    TIME_MON                => { type => 'TIME_MON' },
    TIME_SEC                => { type => 'TIME_SEC' },
    TIME_YEAR               => { type => 'TIME_YEAR' },
    TX                      => { type => 'TX', storage => 1 },
    IP                      => { type => 'IP', storage => 1 },
};

my $valid_operators = {
    beginsWith       => sub { my $pattern = shift; return('REFIND', "^$pattern"); },
    contains         => 'STR_CONTAINS',
    containsWord     => sub { my $pattern = shift; return('REFIND', "\\b$pattern\\b"); },
    detectSQLi       => 'DETECT_SQLI',
    detectXSS        => 'DETECT_XSS',
    endsWith         => sub { my $pattern = shift; return('REFIND', "$pattern\$"); },
    eq               => 'EQUALS',
    ge               => 'GREATER_EQ',
    gt               => 'GREATER',
    ipMatch          => 'CIDR_MATCH',
    ipMatchF         => 'CIDR_MATCH',
    ipMatchFromFile  => 'CIDR_MATCH',
    le               => 'LESS_EQ',
    lt               => 'LESS',
    pm               => 'PM',
    pmf              => 'PM',
    pmFromFile       => 'PM',
    rbl              => 'RBL_LOOKUP',
    rx               => 'REFIND',
    streq            => 'EQUALS',
    strmatch         => 'STR_MATCH',
    within           => 'STR_EXISTS',
};

my $valid_transforms = {
    base64decode       => 'base64_decode',
    base64decodeext    => 'base64_decode',
    base64encode       => 'base64_encode',
    cmdline            => 'cmd_line',
    compresswhitespace => 'compress_whitespace',
    hexdecode          => 'hex_decode',
    hexencode          => 'hex_encode',
    htmlentitydecode   => 'html_decode',
    length             => 'length',
    lowercase          => 'lowercase',
    md5                => 'md5',
    normalisepath      => 'normalise_path',
    removewhitespace   => 'remove_whitespace',
    removecomments     => 'remove_comments',
    removecommentschar => 'remove_comments_char',
    replacecomments    => 'replace_comments',
    sha1               => 'sha1',
    sqlhexdecode       => 'sql_hex_decode',
    trim               => 'trim',
    trimleft           => 'trim_left',
    trimright          => 'trim_right',
    urldecode          => 'uri_decode',
    urldecodeuni       => 'uri_decode',
};

my $action_lookup = {
    allow => 'ACCEPT',
    block => 'DENY',
    deny  => 'DENY',
    drop  => 'DROP',
    pass  => 'IGNORE'
};

my $phase_lookup = {
    1 => 'access',
    2 => 'access',
    3 => 'header_filter',
    4 => 'body_filter',
    5 => 'log',
};

sub meta_exception {
    my ($translation) = @_;

    my $do_add = not defined $translation->{actions}->{nondisrupt};

    $do_add ||= defined $translation->{actions}->{nondisrupt} &&
        ! grep { $_ eq 'rule_remove_by_meta' }
        map { $_->{action} } @{$translation->{actions}->{nondisrupt}};

    if ($do_add) {
        push @{$translation->{actions}->{nondisrupt}},
            {
                action => 'rule_remove_by_meta',
                data   => 1,
            };
    }
}

my $ctl_lookup = {
    ruleRemoveById => sub {                 #ctl:ruleRemoveById=920271,
        my ($value, $translation) = @_;

        push @{$translation->{actions}->{nondisrupt}}, {
            action => 'rule_remove_id',     #取消ID编号为value的规则匹配校验 #跟后面要执行的id有关 盲猜把这个id给临时移除?
            data   => $value,
        };
    },
    ruleRemoveByMsg => sub {
        my ($value, $translation) = @_;

        push @{$translation->{exceptions}}, $value;

        meta_exception($translation);
    },
    ruleRemoveByTag => sub {                #取消所有tag属性包含value的规则的匹配校验
        my ($value, $translation) = @_;

        push @{$translation->{exceptions}}, $value;

        meta_exception($translation);
    },
    #ctl:ruleRemoveTargetByTag=OWASP_CRS;ARGS:user_mail_cancel_confirm_body,\
    # TODO Target的意思是 ARGS中的user_mail_cancel_confirm_body字段将会被忽略 控制的粒度比较细
};

my $op_sep_lookup = {
    PM         => '\s+',
    CIDR_MATCH => ',',
};

my $defaults = {
    action => 'DENY',
    phase  => 'access',
};

our @auto_expand_operators = qw(
    beginsWith
    contains
    containsWord
    endsWith
    eq
    ge
    gt
    le
    lt
    streq
    within
);

my @alters_pattern_operators = qw(beginsWith containsWord endsWith);

sub valid_line {
    my ($line) = @_;

    # the directive must be the first element in the line
    # so if this does not match our whitelist we can't process it
    return any { $line =~ m/^$_ / } @valid_directives;
}

sub clean_input {                       #hankai0 最终返回一个array 里面每个元素代表一条规则
    my (@input) = @_;

    my (@lines, @line_buf);

    for my $line (@input) {             # 首先line字串里没有换行符
        # ignore comments and blank lines
        next if ! $line;
        next if $line =~ m/^\s*$/;
        next if $line =~ m/^\s*#/;

        # trim whitespace
        $line =~ s/^\s*|\s*$//g;

        # merge multi-line directives
        # ex.
        #
        # SecRule \
        #   ARGS \
        #   "foo" \
        #   "t:none, \
        #   block, \
        #   phase:1, \
        #   setvar:tx.foo=bar, \
        #   expirevar:tx.foo=60"
        #
        # strip the multi-line ecape and surrounding whitespace
        if ($line =~ s/\s*\\\s*$//) {   #把反斜杠删掉
            push @line_buf, $line;
        } else {
            # either the end of a multi line directive or a standalone line
            # push the buffer to the return array and clear the buffer
            push @line_buf, $line;

            my $final_line = join ' ', @line_buf;

            push @lines, $final_line if valid_line($final_line);
            @line_buf = ();
        }
    }

    return @lines;
}

# take a line and return an array of tokens representing various rule parts
sub tokenize {                                      #hankai1 token化 #一般把本条规则会拆出来(如下)四部分(并去掉引号) 放到tokens数组里 
    #SecRule
    #REQUEST_FILENAME
    #@endsWith /block/add
    #id:9001206, phase:2, pass, nolog, ctl:ruleRemoveTargetByTag=OWASP_CRS;ARGS:body[0][value], ver:'OWASP_CRS/3.2.0'

    my ($line) = @_;

    my @tokens;

    # so... this sucks
    # we have to make a few assumptions about our line
    # - tokens are whitespace separated
    # - tokens must be quoted with " if they contain spaces
    # - " chars within quoted tokens must be escaped with \
    my $re_quoted   = qr/^"((?:[^"\\]+|\\.)*)"/;    #从"开始匹配 
    #my $re_quoted   = qr/^"(([^"\\]+|\\.)*)"/;     #从"开始匹配 
                                                    #   a)直到匹配到"或者\ 
                                                    #   b)直到匹配到\跟一个任意字符
                                                    #最后再匹配一个"  #即匹配""中的内容 包括转义
    my $re_unquoted = qr/([^\s]+)/;                 #匹配非空格字串
    #print "re_quoted: ", $re_quoted, "\n";         # (?^:^"((?:[^"\\]+|\\.)*)")
    #print "re_unquoted: ", $re_unquoted, "\n";     # (?^:([^\s]+))

    # walk the given string and grab the next token
    # which may be either quoted or unquoted
    # from there, push the token to our list of fields
    # and strip it from the input line
    while ($line =~ $re_quoted || $line =~ $re_unquoted) { #要么是引号里面的内容 要么是单独的一个字串
        #print "\$1: ", $1, "\n";
        #print "line: ", $line, "\n";               #不修改原始的字串
        my $token = $1;                             #以数字为名的变量保存的是上一次匹配操作（/pattern/中 第n个小括号中的原符号所匹配内容 
                                                    #$1就是第一对小括号中的原符号所对应的匹配内容 $2就是第二对小括号中的原符号所对应的匹配内容
        $line =~ s/"?\Q$token\E"?//;                #从原始串中找到token 然后用空格代替之
        #print "line1: ", $line, "\n";
        $line =~ s/^\s*//;                          #移除最前面的空格
        #print "line2: ", $line, "\n";

        # remove any escaping backslashes from escaped quotes
        # e.g. "foo \" bar" becomes literal 'foo " bar'
        $token =~ s/\\"/"/g;                        #把token里的 \" 换为 "
        #print "token: ", $token, "\n";
        push @tokens, $token;
    }
    #print "\n";
    #print join "\n ", map { $_ } @tokens;
    #print "\n";
    #exit(0);
    return @tokens;
}

# take an array of rule parts and return a hashref of parsed tokens
sub parse_tokens {                                  #hankai2 tokens是一个数组 一般包含4个部分(directive + voa 参考hankai1)
    my (@tokens) = @_;

    my ($entry, $directive, $vars, $operator, $actions);
    $entry = {};

    # save this for later debugging / warning
    $entry->{original}  = join ' ', @tokens;        #记录原始规则串

    $directive = shift @tokens;                     #初始化voa
    if ($directive eq 'SecRule') {
        $vars     = shift @tokens;
        $operator = shift @tokens;
    }
    $actions = shift @tokens;

    die "Uh oh! We shouldn't have any fields left but we still have @tokens\n" if @tokens;

    $entry->{directive} = $directive;
    $entry->{vars}      = parse_vars($vars) if $vars; #是一个数组引用
    $entry->{operator}  = parse_operator($operator) if $operator;
    $entry->{actions}   = parse_actions($actions) if $actions;

    return $entry;                                  #该条规则 解析到VOA中
}

sub parse_vars {                                    #hankai2.1
    my ($raw_vars) = @_;                            #获取裸串
    #print $raw_vars, "\n";
    #REQUEST_FILENAME
    #ARGS:destination
    #REQUEST_HEADERS:Content-Type
    #REQUEST_COOKIES:/S?SESS[a-f0-9]+/
    #REQUEST_HEADERS:Content-Length|REQUEST_HEADERS:Transfer-Encoding
    #ARGS:/(foo|bar)/ 
    #ARGS:'/(foo|bar)/'
    #ARGS:/(foo|bar)/|ARGS:'/(foo|bar)/'

    my (@tokens, @parsed_vars, @var_buf, $sentinal);
    #my (@tokens, @var_buf, $sentinal);             #等价
    #my $parsed_vars = [];
    my @split_vars = split /\|/, $raw_vars;         #|暴力分割

    # XXX
    # vars may take one of a few forms
    # standalone: ARGS
    # select a value: ARGS:foo
    # select a regexed value: ARGS:/(foo|bar)/
    # select a regexed value wrapped in single quotes: ARGS:'/(foo|bar)/'
    #
    # note that the ModSecurity documentation does not explicitly cover
    # specifying elements via regex; CRS base rules both with and without
    # quote-wrapped expressions, so we'll handle both
    #
    # because a regexed value may contain a pipe (|)
    # we cant simply split on pipe so we need to
    # loop through and piece together tokens
    while (@split_vars) {
        # take a chunk and add it the buffer array
        # once we know we've reached the end of an
        # action, we'll put the buffer elements
        # back together and add it to the final array
        my $chunk = shift @split_vars;
        push @var_buf, $chunk;

        # we're done chaining together chunks if:
        #
        # - we didnt have the potential to split
        #   meaning that the chunk didnt have a : or regex wrapper
        #   and that the first member of the buffer
        #   did contain a regex wrapper
        #
        # OR
        #
        # - we could have split but we know we're done
        # (we know this if the last member of the chunk is /'?
        $sentinal = 1 if (($chunk !~ m/(?:\/'?|'?\/)/ || $chunk !~ m/:/) &&     #如果chunk中不包含: 或 看不懂
            ! (scalar @var_buf > 1 && $var_buf[0] =~ m/(?:\/'?|'?\/)/))
            || $chunk =~ m/\/'?$/;

        if ($sentinal) {
            push @tokens, join '|', @var_buf;
            @var_buf  = ();
            $sentinal = 0;
        }
    }                                                                           #最终结果是 把v都拆出来 放到tokens数组里

    # [ARGS:/(foo|bar)/] [ARGS:'/(foo|bar)/']
    for my $token (@tokens) {                                                   #遍历 捕获的变量 们
        # variables may take a few forms
        # ! and & are optional metacharacters (mutually exclusive)
        # an optional ':foo' element may also exist
        my ($var, @rest) = split ':', $token;

        my $specific = join ':', @rest;
        my $modifier;

        my $parsed = {};

        # if we see a modifier, strip it from the var
        # and populate its own field                                            # &REQUEST_HEADERS_NAMES:Referer 
                                                                                # !ARGS:/^(?i:ref(erer)?)$/
        if ($var =~ m/^[&!]/) {
            $modifier = substr $var, 0, 1, '';
            $parsed->{modifier} = $modifier;                                    #modifier要么是& 要么是!
        }

        # pop the last var off the stack to add our ignore
                                                                                #ARGS_POST|REQUEST_COOKIES|!REQUEST_COOKIES:/__utm/|REQUEST_COOKIES_NAMES|REQUEST_HEADERS|!REQUEST_HEADERS:/Cookie/|XML:/*
        if (defined $modifier && $modifier eq '!') {                            #如果变量前面有!
            my $prev_parsed_var = pop @parsed_vars;

            if (!$prev_parsed_var) {
                warn "No previous var\n";                                       #!REQUEST_COOKIES:/__utm/|ARGS_NAMES|ARGS|XML:/*     这种写法不允许?
                next;
            }

            if ($prev_parsed_var->{variable} ne $var) {
                warn "Seen var $var doesn't match previous var $prev_parsed_var->{variable}\n";
                push @parsed_vars, $prev_parsed_var;
                next;
            }

            push @{$prev_parsed_var->{ignore}}, $specific;                      # 与前面相同的合并 并添加一个ignore字段
                                                                                #定义ignore数组引用之 
            $parsed = $prev_parsed_var;
            $parsed->{modifier} = '!';
            push @parsed_vars, $parsed;
            next;
        }

        $parsed->{variable} = $var;
        $parsed->{specific} = $specific;

        push @parsed_vars, $parsed;                                             #将这个变量拆出来三部分 放到parsed里
    }

    return \@parsed_vars;                                                       #返回的是一个数组引用
}

sub parse_operator {                                                            #hankai2.2
    my ($raw_operator) = @_;

    # operators may be defined by the @ symbol
    # if one isnt' defined, 'rx' is the default
    # everything following in this token is the pattern
    #
    # using a regex here makes the parser a little more flexible
    # we could split on space, but that breaks if the operator
    # is not single space separated from the pattern, and splitting
    # on \s+ isn't possible because that could break the pattern
    # when joining back together
    #
    # note that some operators (i'm looking at you, libinjection wrapper)
    # do not require a pattern, so we need to account for such cases
    my ($negated, $operator, $pattern) = $raw_operator =~ m/^\s*(?:(\!)?(?:\@([a-zA-Z]+)\s*)?)?(.*)$/;
    $operator ||= 'rx';                                                         #o 默认为rx

    my $parsed = {};

    $parsed->{negated}  = $negated if $negated;
    $parsed->{operator} = $operator;
    $parsed->{pattern}  = $pattern;

    return $parsed;                                                             #返回一个hash引用
}

sub parse_actions {                                                             #hankai 2.3
    my ($raw_actions) = @_;

    my (@tokens, @parsed_actions, @action_buf, $sentinal);
    my @split_actions = split ',', $raw_actions;                                #id:9001216, phase:2, pass, nolog, ctl:ruleRemoveTargetByTag=OWASP_CRS;ARGS:feed_description, ver:'OWASP_CRS/3.2.0', msg:'AspcmsV2.5.6, SQL Injection Attack'

    # actions may take one of a few forms
    # standalone: deny
    # express a value: phase:1
    # express a quoted value: msg:'foo bar'
    #
    # because the quoted value in something like msg or logdata
    # may have commas, we cant simply split on comma
    # so we need to loop through and piece together tokens
    while (@split_actions) {
        # take a chunk and add it the buffer array
        # once we know we've reached the end of an
        # action, we'll put the buffer elements
        # back together and add it to the final array
        my $chunk = shift @split_actions;                                       #从数组中移出来放临时数组
        push @action_buf, $chunk;

        # we're done chaining together chunks if:
        #
        # - we didnt have the potential to split
        #   meaning that the chunk didnt have a : or '
        #   and that the first member of the buffer
        #   did contain a '
        #
        # OR
        #
        # - we could have split but we know we're done
        # (we know this if the last member of the chunk is a ')
                                                                                # msg:'AspcmsV2.5.6, SQL Injection Attack'
                                                                                # ----------------- 这部分已经翻入临时数组
        $sentinal = 1 if (($chunk !~ m/'/ || $chunk !~ m/:/)                    # 如果刚刚放到临时数组中的chunk 没有'或者: 且 临时数组中最初的元素没有' 则sentinal为1 即可以收割
            && ! (scalar @action_buf > 1 && $action_buf[0] =~ m/'/))            # 刚刚放进去的元素是正常的(还要看以前最初的元素是否正常) || 最后一个字符是'
            || $chunk =~ m/'$/;

        if ($sentinal) {
            push @tokens, join ',', @action_buf;
            @action_buf  = ();
            $sentinal = 0;
        }
    }                                                                           #action_buf的作用是 "提纯"action

    # great, now that we have proper tokens
    # we can split any potential key value pairs
    # and add them to the final array
    for my $token (@tokens) {                                                   #tokens数组里是 经过提纯后的action集
        my ($action, @value) = split /:/, $token;                               #如果是kv类型的action则

        # trim whitespace (this is necessary for multi-line rules)
        $action =~ s/^\s*|\s*$//g;

        my $parsed = {};

        $parsed->{action}   = $action;
        $parsed->{value} = strip_encap_quotes(join ':', @value) if @value;

        push @parsed_actions, $parsed;                                          #这里用的是数组存的
    }

    return \@parsed_actions;
}

# strip encapsulating single or double quotes from a string
sub strip_encap_quotes {
    my ($line) = @_;

    $line =~ s/^(['"])(.*)\1$/$2/;                                              #萃取单双引号里的内容
                                                                                #\1即 匹配组1中的内容 即也是单引号或双引号

    return $line;
}

# take an array of rule hashrefs and return an array of chain hashrefs
sub build_chains {
    my (@rules) = @_;

    my (@chain, @chains);

    for my $rule (@rules) {
        push @chain, $rule;                                                     #先将这条规则放到临时数组中

        # figure if this rule is part of a chain
        next if grep { $_ eq 'chain' } map { $_->{action} } @{$rule->{actions}};#如果这条规则的action中有chain关键字 则continue

        # if the chain action isnt set, we're either a standalone rule
        # or at the end of a chain; either way, push this chain
        # to our array of chains and empty the current chain buffer
        push @chains, [ @chain ];                                               #数组里是 匿名数组 匿名数组里是多个或单个规则
        @chain = ();
    }

    return @chains;
}

# take an array of ModSecurity chain hashrefs and return a hashref of lua-resty-waf rule hashrefs (including appropriate phases)
sub translate_chains {
    my ($args) = @_;
    my @chains = @{$args->{chains}};
    my $quiet  = $args->{quiet};
    my $silent = $args->{silent};
    my $force  = $args->{force};
    my $path   = $args->{path};

    my $lua_resty_waf_chains = {
        access        => [],
        header_filter => [],
        body_filter   => [],
    };

    for my $chain (@chains) {                                                   # ([rule1], [rule2], [rule3.1, rule3.2], [rule4])
        try {
            my @translation = translate_chain({
                chain  => $chain,
                silent => $silent,
                force  => $force,
                path   => $path,
            });

            my $phase = figure_phase(@translation);

            push @{$lua_resty_waf_chains->{$phase}}, @translation;
        } catch {
            warn $_ if !$silent;

            if (!$quiet) {
                warn $_->{original} . "\n" for @{$chain};
                print "\n\n";
            }
        };
    }

    return $lua_resty_waf_chains;
}

# take an array of hashrefs representing modsec rules and return an array of
# hashrefs representing lua-resty-waf rules. if the translation cannot be performed
# due to an imcompatability, die with the incompatible elements
sub translate_chain {
    my ($args) = @_;
    my @chain  = @{$args->{chain}};                                 # chain是一个匿名数组 匿名数组里是单条规则或多条规则组成的chain
    my $silent = $args->{silent};
    my $force  = $args->{force};
    my $path   = $args->{path};

    my (@lua_resty_waf_chain, $chain_id, $chain_action, $ctr);

    my @end_actions = qw(action msg logdata skip skip_after);
    #my $var = 0;

    for my $rule (@chain) {
        #print "-------------------var: ", $var, "\n";
        #$var += 1;
        my $translation = {};

        if ($rule->{directive} eq 'SecRule') {
            translate_vars($rule, $translation, $force);
            translate_operator($rule, $translation, $path);
        } elsif ($rule->{directive} =~ m/Sec(?:Action|Marker)/) {
            push @{$translation->{vars}}, { unconditional => 1 };

            # SecMarker is a rule that never matches
            # with its only action representing its ID
            if ($rule->{directive} eq 'SecMarker') {
                $translation->{op_negated} = 1;
                my $marker = pop @{$rule->{actions}};
                $translation->{id} = $marker->{action};
            }
        }

        translate_actions($rule, $translation, $silent);

        # assign the same ID to each rule in the chain
        # it is guaranteed that the first rule in a
        # ModSecurity chain must have a valid, unique ID
        # lua-resty-waf only requires that each rule has an ID,
        # not that each rule's ID must be unique
        $chain_id = $translation->{id} if $translation->{id};
        $translation->{id} = $chain_id if ! $translation->{id};     #如果子链没有id则复用父链的id

        # these actions exist in the chain starter in ModSecurity
        # but they belong in the final rule in lua-resty-waf
        for my $action (@end_actions) {                             #只有最后的action生效
            $chain_action->{$action} = delete $translation->{$action} if $translation->{$action};
        }

        # if we've reached the end of the chain, assign our values that
        # had to be pushed from the chain starter, or assign the default
        # if the chain starter didn't specify. otherwise, we're at the start
        # or middle of a chain, so the only thing we know to do is set the CHAIN action
        if (++$ctr == scalar @chain) {                              #数组的最后一个
            for my $action (@end_actions) {
                if ($chain_action->{$action}) {
                    $translation->{$action} = $chain_action->{$action};
                } elsif ($defaults->{$action}) {
                    $translation->{$action} = $defaults->{$action};
                }
            }
        } else {
            $translation->{action} = 'CHAIN';
        }

        $translation->{actions}->{disrupt} = delete $translation->{action};

        push @lua_resty_waf_chain, $translation;
    }

    return @lua_resty_waf_chain;
}

sub translate_vars {
    my ($rule, $translation, $force) = @_;

    # maintain a 1-1 translation of modsec vars to lua-resty-waf vars
    # this necessitates that a lua-resty-waf rule vars key is an array
    for my $var (@{$rule->{vars}}) {                                    #参考hankai2.1
        my $original_var = $var->{variable};
        my $lookup_var   = clone($valid_vars->{$original_var});         #从字典中查找该变量名 #深copy
                                                                        #lookup_var = { type => 'REQUEST_ARGS', parse => [ qw(values 1) ] },

        die "Cannot translate variable $original_var" if !$lookup_var && !$force;
        next if !$lookup_var;

        die "Cannot have a specific attribute when the lookup table already provided one"
            if ($var->{specific} && grep { $_ eq 'specific' } @{$lookup_var->{parse}});
                                                                        #如果指定了要捕获的变量中的某个元素  # hankai TODO
                                                                        # ARGS:/(foo|bar)/|ARGS:'/(foo|bar)/'
                                                                        # !ARGS:/^(?i:ref(erer)?)$/

        my $translated_var = $lookup_var;
        my $modifier       = $var->{modifier};
        my $specific       = $var->{specific};

        my $specific_regex;
        if ($specific =~ m/^'?\//) {                                    #如果是'/开头 或者 /开头的字符串
            $specific =~ s/^'?\/(.*)\/'?/$1/;                           #则去掉这部分 eg: '/(foo|bar)/' 变成了 (foo|bar)
            $specific_regex = 1;
        }

        if (defined $modifier && $modifier eq '!') {                    #如果有! 则说明有ignore字段
            for my $elt (@{$var->{ignore}}) {
                my $elt_regex;
                if ($elt =~ m/^'?\//) {
                    $elt =~ s/^'?\/(.*)\/'?/$1/;
                    $elt_regex = 1;
                }

                my $key = $elt_regex ? 'regex' : 'ignore';              #这里的ignore起的名字不太好 应该起string好一些 表示的是要忽略的字符串

                $elt = uc $elt if $lookup_var->{storage};               #hankai TODO

                push @{$translated_var->{ignore}}, [ ($key, $elt) ];
                                                                        #lookup_var = { type => 'REQUEST_ARGS', parse => [ qw(values 1) ], ignore => [ regex, (foo|bar) ] },
                                                                        #lookup_var = { type => 'REQUEST_ARGS', parse => [ qw(values 1) ], ignore => [[ regex, (foo|bar) ], [ ignore, '(foo|bar)']] },
            }
        } elsif (length $specific) {
            my $key = $specific_regex ? 'regex' : 'specific';           #名字起的不好 同上

            $specific = uc $specific if $lookup_var->{storage};         #hankai TODO

            delete $translated_var->{parse};
            push @{$translated_var->{parse}}, $key, $specific;
                                                                        #lookup_var = { type => 'REQUEST_ARGS', parse => [ regex, (foo|bar), specific, '(foo|bar)' ] },
        }

        if (defined $modifier && $modifier eq '&') {
            $translated_var->{length} = 1;
        }
                                                                        #lookup_var = { type => 'REQUEST_ARGS', parse => [ regex, (foo|bar), specific, '(foo|bar)' ], length => 1 },

        push @{$translation->{vars}}, $translated_var;                  #将这一个匿名的var结构放到vars引用的数组中去
    }

    return;
}

sub translate_operator {
    my ($rule, $translation, $path) = @_;

    my $original_operator   = $rule->{operator}->{operator};
    my $translated_operator = $valid_operators->{$original_operator};

    die "Cannot translate operator $original_operator"
        if !$translated_operator;

    # in some cases its easier to have our translation alter the pattern
    # rather than create separate-but-mostly equal operators
    # in these cases the lookup table gives us a function we can use
    # to get both the operator and the altered pattern
    if (any { $_ eq $original_operator } @alters_pattern_operators) {   #beginsWith       => sub { my $pattern = shift; return('REFIND', "^$pattern"); },
                                                                        #如果命中特殊的三剑客
        my ($operator, $pattern) = $translated_operator->($rule->{operator}->{pattern});
        $translation->{operator} = $operator;
        $translation->{pattern}  = $pattern;
    } else {
        $translation->{operator} = $translated_operator;
        $translation->{pattern}  = $rule->{operator}->{pattern};
    }

    $translation->{op_negated} = 1 if $rule->{operator}->{negated};

    # force int
    $translation->{pattern} += 0 if $translation->{pattern} =~  m/^\d*(?:\.\d+)?$/;
                                                                        #如果是纯数字则强转为int/float?

    # this operator reads from a file.
    # read the file and build the pattern table
    # n.b. this regex is very simple, we rely on the
    # fact that no other support operators end with the
    # letter 'f' in order to support this. thanks SpiderLabs.
    # see https://github.com/SpiderLabs/ModSecurity/wiki/Reference-Manual#pmf
    # and https://github.com/SpiderLabs/ModSecurity/wiki/Reference-Manual#ipmatchf
    if ($rule->{operator}->{operator} =~ m/[fF]$|FromFile$/) {
        my @buffer;
        my $pattern_file = $rule->{operator}->{pattern};

        $path ||= '.'; # if a path wasn't given, check in the current dir

        open my $fh, '<', "$path/$pattern_file" or die $!;

        # read the FromFile target and build out the pattern based on its contents
        while (my $line = <$fh>) {
            chomp $line;

            # ignore comments and blank lines
            next if $line =~ m/^\s*$/;
            next if $line =~ m/^\s*#/;

            push @buffer, $line;
        }

        close $fh;

        $translation->{pattern} = [ @buffer ];
        return;
    }

    # some operators that behave on split patterns need to be adjusted
    # as lua-resty-waf will expect the pattern as a table
    if (my $special_op = $op_sep_lookup->{$translated_operator}) {
        my @pattern = split /$special_op/, $rule->{operator}->{pattern};
        $translation->{pattern} = \@pattern;
    }

    # automatically expand the rule pattern for certain operators
    if (any { $_ eq $original_operator } @auto_expand_operators) {
        $translation->{opts}->{parsepattern} = 1;
        $translation->{pattern} = translate_macro($translation->{pattern});
    }

    return;
}

sub translate_actions {
    my ($rule, $translation, $silent) = @_;

    my @silent_actions = qw(
        chain
    );

    my @disruptive_actions = qw(
        allow
        block
        deny
        drop
        pass
    );

    my @direct_translation_actions = qw(
        accuracy
        id
        maturity
        phase
        rev
        severity
        skip
        ver
    );

    for my $action (@{$rule->{actions}}) {
        my $key   = $action->{action};
        my $value = $action->{value};
        #print "action, ", $key, ": ", $value, "\n";

        # these values have no direct translation,
        # but we don't need to warn about them
        next if grep { $_ eq $key } @silent_actions;                    #忽略chain关键字

        # easier to do this than a lookup table
        if (grep { $_ eq $key } @disruptive_actions) {
            $translation->{action} = uc $action_lookup->{$key};         #格式化拦截动作
        } elsif (grep { $_ eq $key } @direct_translation_actions) {
            $translation->{$key} = $value;
        } elsif ($key eq 'capture') {                                   #将捕获结果存入tx变量 可以存储10个变量 tx下标为0~9
            $translation->{operator} eq 'REFIND' ?                      #如果捕获变量的话 那么就得把refind正则命令 转成regex
                $translation->{operator} = 'REGEX' :
                warn 'capture set when translated operator was not REFIND';
        } elsif ($key eq 'ctl') {
            print "action, ", $key, ": ", $value, "\n";                 # ruleRemoveTargetByTag=OWASP_CRS;ARGS:body[0][value]
            my ($opt, $data) = split /=/, $value;

            if (defined $ctl_lookup->{$opt}) {
                $ctl_lookup->{$opt}($data, $translation);
            } else {
                warn "Cannot translate ctl option $opt";
            }
        } elsif ($key eq 'expirevar') {                                 # expirevar:TX.suspicious_activity_detected 把suspicious_activity_detected这个值从当前上下文删除
                                                                        # setvar:session.suspicious=1, expirevar:session.suspicious=3600, 设置过期时间?
            my ($var, $time)           = split /=/, $value;
            my ($collection, $element) = split /\./, $var;

            # dont cast as an int if this is a macro
            $time = $time =~ m/^\d*(?:\.\d+)?$/ ? $time + 0 : translate_macro($time);

            push @{$translation->{actions}->{nondisrupt}},
                {
                    action => 'expirevar',
                    data   => {
                        col  => uc $collection,
                        key  => uc $element,
                        time => $time,
                    }
                };
        } elsif ($key eq 'initcol') {
            my ($col, $val) = split /=/, $value;

            push @{$translation->{actions}->{nondisrupt}},
                {
                    action => 'initcol',
                    data   => {
                        col   => uc $col,
                        value => translate_macro($val),
                    }
                };
        } elsif ($key eq 'logdata') {                                   #logdata:'Matched pattern: %{MATCHED_VAR}' # MATCHED_VAR即$valid_vars中的key
            $translation->{logdata} = translate_macro($value);
        } elsif ($key eq 'msg') {
            $translation->{msg} = translate_macro($value);
        } elsif ($key =~ m/^no(?:audit)?log$/) {
            $translation->{opts}->{nolog} = 1;
        } elsif ($key =~ m/^(?:audit)?log$/) {
            delete $translation->{opts}->{nolog} if defined $translation->{opts};
        } elsif ($key eq 'skipAfter') {
            $translation->{skip_after} = $value;
        } elsif ($key eq 'setvar') {                                    #setvar:tx.score=+5
                                                                        #setvar:tx.anomaly_score=+%{tx.critical_anomaly_score}
                                                                        #setvar:'tx.msg=%{rule.msg}',  
                                                                        #setvar:tx.%{rule.id}-OWASP_CRS/WEB_ATTACK/SQL_INJECTION-%{matched_var_name}=%{matched_var}"
            my ($var, $val)            = split /=/, $value;
            my ($collection, @elements) = split /\./, $var;

            my $element = join '.', @elements;

            # no $val, perhaps a delete?
            if (! defined $val) {                                       #setvar:!tx.score #删除变量
                if ($var =~ m/^\!/) {
                    substr $collection, 0, 1, '';                       #剔除!

                    push @{$translation->{actions}->{nondisrupt}}, {
                        action => 'deletevar',
                        data   => {
                            col => uc $collection,
                            key => uc $element,
                        }
                    };
                } else {
                    warn "No assignment in setvar, but not a delete?\n";
                }
                next;
            }

            my $setvar = { col => uc $collection, key => uc $element }; #{col => TX, key => score }

            if ($val =~ m/^\+/) {
                substr $val, 0, 1, '';
                $setvar->{inc} = 1;                                     #{col => TX, key => score, inc => 1 };
            }

            if ($val =~ m/^\d*(?:\.\d+)?$/) {
                $val += 0;                                              #val强转整型
            } else {
                $val = translate_macro($val);                           #{col => TX, key => score, inc => 1, val => 111 };
            }                                                           #{col => TX, key => score, inc => 1, val => %{rule.id}-%{matched_var_name} };

            $setvar->{value} = $val;

            push @{$translation->{actions}->{nondisrupt}},
                {
                    action => 'setvar',
                    data   => $setvar
                };
        } elsif ($key eq 'status') {
            push @{$translation->{actions}->{nondisrupt}},
                {
                    action => $key,
                    data   => $value += 0,
                };
        } elsif ($key eq 'pause') {
            my $time = $value / 1000; # pause:n is given in ms, ngx.sleep takes its arg as seconds

            push @{$translation->{actions}->{nondisrupt}},
                {
                    action => 'sleep',
                    data   => $time,
                };
        } elsif ($key eq 't') {
            next if $value eq 'none';

            my $transform = $valid_transforms->{lc $value};

            if (!$transform) {
                warn "Cannot perform transform $value" if !$silent;
                next;
            }

            push @{$translation->{opts}->{transform}}, $transform;
        } elsif ($key eq 'tag') {
            push @{$translation->{tag}}, translate_macro($value);
        } else {
            warn "Cannot translate action $key\n" if !$silent;
        }
    }

    return;
}

sub figure_phase {
    my ($translation) = @_; # grab only the first element

    # phase must be defined in the first rule of the chain
    my $phase = delete $translation->{phase};

    return $phase && defined $phase_lookup->{$phase} ? $phase_lookup->{$phase} : $defaults->{phase};
}

# because we don't maintain a strict 1-1 mapping of collection names
# we need to fudge the contents of macros. note this is not actually
# performing the expansion (that happens at runtime by the rule engine),
# we're merely updating the string to accomodate lua-resty-waf's lookup table
sub translate_macro {
    my ($string) = @_;

    # grab each macro and replace it with its lookup equivalent
    for my $macro ($string =~ m/%{([^}]+)}/g) {                     #匹配出所有形如 %{ARGS.test0} %{SERVER_NAME} 的字符串
        my ($key, $specific) = split /\./, $macro;
        my $replacement;

        if (my $lookup = clone($valid_vars->{uc $key})) {           #$lookup = { type => 'REQUEST_ARGS', parse => [ qw(values 1) ] },
                                                                    #$lookup = { type => 'REQUEST_HEADERS', parse => [ qw(specific Host) ] },
            $replacement = $lookup->{type};                         #

            if ($lookup->{storage}) {
                # this is a pretty ugly hack...
                $replacement .= '.' . uc $specific;
            } else {
                $replacement .= ".$lookup->{parse}->[1]"            #REQUEST_HEADERS.Host
                    if $lookup->{parse}->[0] && $lookup->{parse}->[0] eq 'specific';

                $replacement .= ".$specific" if $specific;          #ARGS.test0 或者 REQUEST_HEADERS.Host
            }
        } else {
            $replacement = $macro;
        }

        $replacement = "%{$replacement}";

        $string =~ s/\Q%{$macro}\E/$replacement/g;
    };

    return $string;
}

1;
