1���泣�� literal
2��������
    const Pi float64 = 3.1415926
    const zero = 0.0 //�����͵ĸ��㳣��
    const {
        size int64 = 1024
        eof = -1 //�����͵����γ���
    }
    const u, v, float32 = 0, 3 //�����Ķ��ظ�ֵ
    const a, b, c = 3, 4, "foo" //���������κ��ַ�������

    �����ĸ�ֵ��һ�������ڼ���Ϊ ������ֵ���ܳ����κ������в��ܵó�����ı��ʽ
    eg const home = os.GetEnv("HOME")
3Ԥ���峣��
    true false iota
    iota��һ���ɱ��������޸ĵĳ�������ÿһ��const�ؼ��ֳ���ʱ������0 ����һ�γ���const֮ǰ
    ÿ����һ��iota ��������������Զ�++
    const {
        c0 = iota  //0
        c1 = iota  //1  //c1��������ݿ�ʡ�Բ�д
        c2 = iota  //2
    }
    const x = iota //xΪ0

4ö��
    const {
        Sunday = iota
        Monday
        Tuesday
        Wednesday
        Thursday
        Friday
        Saturday
        numberOfDays //û�е���
    }
    ��д��ĸ��ͷ�ĳ����ڰ���ɼ� ��numberOfDays˽��


