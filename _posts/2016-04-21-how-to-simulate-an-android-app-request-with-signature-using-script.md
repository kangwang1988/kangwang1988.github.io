---
layout: post
comments: true
section-type: post
title: 如何用脚本模拟Android APP内带签名的网络请求
category: tech
tags: [ 'tutorial' ]
---

### Content
	1.Warning
	2.Fetch a http/https request
	3.Decompile & Analyze an APK
	4.Write a script to simulate it
	5.Summary
	
### Warning
	
	本文仅限于学习研究所用，严禁使用此中所述方法来做违法乱纪的事情，小心警察蜀黎请你喝咖啡。

### Fetch a http/https request

	使用最新版的Charles，设置其可以截取https的请求。将需要模拟的APK安装到一台Android设备上，随便输入一个用户名和密码，点击登录，可以得到如下所示的网络请求:
	http://api.host.cn/gateway?app_ver=1&appkey=2&from=internal&imei=864690026308934&mac=d4%3A97%3A0b%3A4f%3Af0%3Adc&method=d.login&model=Xiaomi-MI%203W&os=Xiaomi19%2C4.4.4&passwd=e10adc3949ba59abbe56e057f20f883e&sim=460018125610392&timestamp=2016-04-21%2020%3A23%3A52&token=&user=H1&ver=1&version_code=1&sig=0c226ef32682338a05a03a28345b04cf
	针对每一个分析不难看出，固定的值包括:app_ver,appkey,from，ver,version_code;设备相关的值包括imei,model,os;用户相关的参数包括user,passwd;其他的参数包括:timestamp,token,sig。
	针对每一个参数作分析，app_ver,appkey,from,imei,method,sim,ver,version_code均为明文，部分参数由于格式的问题做了urlencode,解码可得:mac=d4:97:0b:4f:f0:dc，model=Xiaomi-MI 3W，timestamp=2016-04-21 19:57:00。token一开始并不存在，登陆成功后将由服务端返回。sig的分析最麻烦，也是分析反编译所得源码的核心。
	
### Decompile & Analyze an APK
下载[jadx-gui](https://github.com/skylot/jadx)
	
	编译结束后打开jadx-gui,对其反编译。因为java的混淆只是自己书写的类的名称和方法，对于常连字符串等并无特殊处理，因此我们可以直接搜索如"sig="等特殊字符串，找到签名所在的代码位置，然后结合上下文，找到sig具体生成的算法。
	常见的sig包括两阶段，第一阶段将参数使用的key/value键值对按照某种方式连接，第二阶段对第一阶段生成的字符串进行散列处理。
	Android反编译得到源码后，可以找到两个阶段的生成方式，在笔者研究的APP中，第一阶段生成了key1value1key2value2...keynvaluen，第二阶段的代码直接拷贝源码过来调用就可以了。
	
### Write a script to simulate it
	
	Using a shell would help you complete this job. For me, I write a java class for it.
	
	public class KWHelper {
	public static String salt = "012";
	public static void main(String argv[]){
		TreeMap<String,String> map = new TreeMap<String,String>();
		map.put("app_ver", "1");
		map.put("appkey", "2");
		map.put("from", "internal");
		map.put("imei", "864690026308934");
		map.put("mac", "d4:97:0b:4f:f0:dc");
		map.put("method", "d.login");
		map.put("model", "Xiaomi-MI 3W");
		map.put("os", "Xiaomi19,4.4.4");
		map.put("passwd", "e10adc3949ba59abbe56e057f20f883e");
		map.put("sim", "460018125610392");
		Date date = new Date();
		SimpleDateFormat sdFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		map.put("timestamp", sdFormat.format(date));
		map.put("token", "");
		map.put("user", "H1");
		map.put("ver", "1");
		map.put("version_code", "1");
		StringBuilder builder = new StringBuilder("http://api.host.cn/gateway?");
		for(Entry<String,String> entry: map.entrySet()){
			builder.append((String)entry.getKey()).append("=").append(entry.getValue().length()==0?"":rawUrlEncode(entry.getValue(),"UTF-8")).append("&");
		}
		builder.append("sig="+signature(map));
		System.out.println(builder.toString());
		}
		
		public static String signature(TreeMap<String,String> aParamsMap){
        StringBuilder stringBuilder = new StringBuilder();
        for (Entry entry : aParamsMap.entrySet()) {
            if (!"sig".equals(entry.getKey())) {
                stringBuilder.append((String) entry.getKey()).append(((String) entry.getValue()).length()==0 ? "" : (String) entry.getValue());
            }
        }
        stringBuilder.append(salt);
        return digestString(stringBuilder.toString());
    	}
	}
	
	其中digestString()方法可以直接从apk反编译得到java实现。
	Done.
	
### Summary
	移动端APP的信息安全是一个难题。如果不涉及到金融等敏感信息，只需要做到简单的md5，url签名就可以了。如果涉及到很敏感的信息，如计算sig所依赖的参数salt等，需要避免明文保存。可以考虑将敏感的内容加密存储到某些特殊的文件或者地方，然后使用之前解密存入内存使用。
	重点还在于服务端，首先要对请求限流，例如使用gateway等的方式对用户在单位时间内的请求次数加以限制。其次业务提供方面对登录等的敏感接口，错误请求到达一定次数后限制访问，且不返回包含明确错误原因的信息。
