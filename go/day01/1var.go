
1��������
    var v1 int
    var v2 string
    var v3 [10]int  //����
    var v4 []int    //������Ƭ
    var v5 struct {
        f int
    }
    var v6 *int //ָ��
    var v7 map[string]int //map keyΪstring value��int
    var v8 fun(a int) int

    ����д����
    var {
        v1 int
        v2 string
    }

2������ʼ��
    var v1 int = 10
    var v2 = 10 //�������Զ��Ƶ�v2Ϊint
    v3 := 10  //ͬ��

    var i int
    i := 10 ����дʱ�����

3������ֵ
    var v10 int
    v10 = 123

    ���ظ�ֵ��
        i, j = j, i  //i j �໥����Ԫ��

4��������
    func get_name() ( first_name, last_name, nick_name string ) {
        return "kai", "han", "goodboy"
    }
    ��ֻ���ȡnick_name:
        _, _, nick_name := get_name()

