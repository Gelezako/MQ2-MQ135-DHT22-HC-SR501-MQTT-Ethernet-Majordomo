/*
* MajordomoAPIAI.Implementation.php
* @author Alex Sokolov <admin@gelezako.com>
* @copyright Alex Sokolov http://www.blog.gelezako.com (c)
* @version 0.1 ([May 23, 2017])
*/

if($params['gas']=="воздуха"){
 	if(gg("Kitchen.MQ2")<100) say("Качество воздуха отличное",2);
 	else if(gg("Kitchen.MQ2")>100 and gg("Kitchen.MQ2")<=200) say("Качество воздуха среднее, не мешало бы проветрить",2);
 	else if(gg("Kitchen.MQ2")>200) say("Качество воздуха плохое, нужно срочно проветрить",2);
}

if($params['gas']=="угарного газа"){
 	$gas="Уровень угарного газа ".gg("Kitchen.MQ2");
 	say("$gas",2);
}

