nft add rule bridge filter forward ether type ip tcp dport 22 accept
nft add rule bridge filter forward ether type arp accept

# table -> chain -> rule
nft add table bridge filter
nft add chain bridge filter forward '{type filter hook forward priority 0;}'
nft add rule bridge filter forward ct state established,related accept
nft add rule bridge filter forward iif {ens37, ens38} oif ens33 ct state new accept
nft add rule bridge filter forward iif ens33 oif ens38 ct state new tcp dport {22, 80, 443} accept

#add table #table必须绑定某个协议族
nft add table ip filter
nft list tables
nft delete table ip foo
nft flush table ip filter

#add base chain into table 
#attach chain into netfilter hook #两步走 1)添加base链  2)挂到input钩子上
nft add chain ip foo base1 '{type filter hook input priority 0 ;}'
nft add chain ip foo input1 '{type filter hook input priority 10 ;}'
nft list table ip foo

