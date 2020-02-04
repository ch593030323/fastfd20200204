-- MySQL dump 10.13  Distrib 5.5.62, for debian-linux-gnu (x86_64)
--
-- Host: 192.168.1.160    Database: hddpos
-- ------------------------------------------------------
-- Server version	5.5.58-0+deb8u1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `t_m_member`
--

DROP TABLE IF EXISTS `t_m_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_member` (
  `vch_memberno` varchar(20) NOT NULL,
  `vch_member` varchar(40) NOT NULL,
  `ch_typeno` varchar(2) DEFAULT NULL,
  `vch_id` varchar(20) DEFAULT NULL,
  `dt_birthday` datetime DEFAULT NULL,
  `vch_tel` varchar(20) DEFAULT NULL,
  `vch_handtel` varchar(20) DEFAULT NULL,
  `vch_address` varchar(50) DEFAULT NULL,
  `dt_limit` datetime DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT '1',
  `num_limit` decimal(12,2) DEFAULT NULL,
  `ch_cardflag` varchar(1) DEFAULT 'N',
  `vch_cardno` varchar(20) DEFAULT NULL,
  `vch_password` varchar(20) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `int_basedata` int(11) DEFAULT '0',
  `dt_sendtime` datetime DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`vch_memberno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_member`
--

LOCK TABLES `t_m_member` WRITE;
/*!40000 ALTER TABLE `t_m_member` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_member` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_member_insert AFTER INSERT ON t_m_member FOR EACH ROW  Begin  delete from del_t_m_member where vch_memberno=NEW.vch_memberno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_member_update BEFORE UPDATE ON t_m_member FOR EACH ROW  Begin   IF(NEW.vch_memberno = OLD.vch_memberno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER `tr_m_member_update` AFTER UPDATE ON `t_m_member` FOR EACH ROW BEGIN
  DECLARE  ls_cardid char(20);     
  DECLARE  ls_branch_no char(6);
  DECLARE  ls_var_value varchar(255);

	set ls_branch_no = '0000';
		IF (NEW.ch_cardflag = 'Y' ) THEN
		
			set ls_cardid =  NEW.vch_memberno ;

			set ls_var_value = (select ifnull(sys_var_value,'')  from mess_t_sys_system where Upper(LTRIM(RTRIM(sys_var_id))) = Upper(LTRIM(RTRIM('vip_new'))));
				   IF (left(ls_var_value,1) = '1' ) THEN
						  insert into mess_t_vip_message(mess_type,branch_no,card_id,father_card,send_flag)	values('3',ls_branch_no,ls_cardid,'','0');
           END IF;
    END IF;
END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_member_delete BEFORE DELETE ON t_m_member FOR EACH ROW  Begin  replace into del_t_m_member select * from t_m_member where vch_memberno=OLD.vch_memberno; update del_t_m_member set ch_yun_already_keep = 'N' where vch_memberno=OLD.vch_memberno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `t_m_curamount`
--

DROP TABLE IF EXISTS `t_m_curamount`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_curamount` (
  `vch_memberno` varchar(20) NOT NULL,
  `num_amount` decimal(12,2) DEFAULT NULL,
  `num_hangamount` decimal(12,2) DEFAULT NULL,
  `num_point` decimal(12,2) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_branchno` varchar(4) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`vch_memberno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_curamount`
--

LOCK TABLES `t_m_curamount` WRITE;
/*!40000 ALTER TABLE `t_m_curamount` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_curamount` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_curamount_insert AFTER INSERT ON t_m_curamount FOR EACH ROW  Begin  delete from del_t_m_curamount where vch_memberno=NEW.vch_memberno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_curamount_update BEFORE UPDATE ON t_m_curamount FOR EACH ROW  Begin   IF(NEW.vch_memberno = OLD.vch_memberno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_curamount_delete BEFORE DELETE ON t_m_curamount FOR EACH ROW  Begin  replace into del_t_m_curamount select * from t_m_curamount where vch_memberno=OLD.vch_memberno; update del_t_m_curamount set ch_yun_already_keep = 'N' where vch_memberno=OLD.vch_memberno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_dish`
--

DROP TABLE IF EXISTS `cey_bt_dish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_dish` (
  `ch_dishno` varchar(7) NOT NULL,
  `vch_dishname` varchar(60) NOT NULL,
  `vch_spell` varchar(40) DEFAULT NULL,
  `vch_english` varchar(40) DEFAULT NULL,
  `ch_dish_typeno` varchar(2) DEFAULT NULL,
  `ch_unitno` varchar(2) DEFAULT NULL,
  `num_price` decimal(12,2) DEFAULT '0.00',
  `ch_suitflag` varchar(1) DEFAULT 'N',
  `ch_discount` varchar(1) DEFAULT 'N',
  `ch_curprice` varchar(1) DEFAULT 'N',
  `ch_tempflag` varchar(1) DEFAULT 'N',
  `ch_stopflag` varchar(1) DEFAULT NULL,
  `ch_printflag` varchar(1) DEFAULT NULL,
  `vch_printer` varchar(50) DEFAULT NULL,
  `ch_sub_typeno` varchar(4) DEFAULT NULL,
  `ch_materialno` varchar(7) DEFAULT NULL,
  `ch_serviceflag` varchar(1) DEFAULT NULL,
  `ch_lowflag` varchar(1) DEFAULT NULL,
  `num_m_price` decimal(12,2) DEFAULT '0.00',
  `ch_deductflag` varchar(1) DEFAULT NULL,
  `ch_deducttype` varchar(1) DEFAULT NULL,
  `int_deduct` int(11) DEFAULT NULL,
  `num_deduct` decimal(12,2) DEFAULT NULL,
  `ch_saleflag` varchar(1) DEFAULT NULL,
  `ch_saletype` varchar(1) DEFAULT NULL,
  `int_sale_deduct` int(11) DEFAULT NULL,
  `num_sale` decimal(12,2) DEFAULT NULL,
  `item_flag` varchar(1) DEFAULT NULL,
  `vch_pur_unitno` varchar(2) DEFAULT NULL,
  `int_unit_rate` int(11) DEFAULT NULL,
  `ch_off` varchar(1) DEFAULT 'N',
  `vch_subno` varchar(40) DEFAULT NULL,
  `num_sale_price` decimal(20,2) DEFAULT '0.00',
  `ch_outflag` varchar(1) DEFAULT NULL,
  `vch_outprint` varchar(50) DEFAULT NULL,
  `int_rate` decimal(12,2) DEFAULT NULL,
  `ch_weight` varchar(1) DEFAULT NULL,
  `vch_barcode` varchar(64) DEFAULT NULL,
  `ch_kitchen_print_flag` varchar(1) DEFAULT NULL,
  `vch_kitchen_print_id1` varchar(2) DEFAULT NULL,
  `vch_kitchen_print_id2` varchar(2) DEFAULT NULL,
  `vch_kitchen_print_id3` varchar(2) DEFAULT NULL,
  `ch_kitchen_out_flag` varchar(1) DEFAULT NULL,
  `vch_kitchen_out_id1` varchar(2) DEFAULT NULL,
  `vch_kitchen_out_id2` varchar(2) DEFAULT NULL,
  `vch_kitchen_out_id3` varchar(2) DEFAULT NULL,
  `ch_groupno` varchar(7) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `num_item_warn` decimal(12,2) DEFAULT '0.00',
  `ch_guqing_flag` varchar(1) DEFAULT NULL,
  `num_guqing_num` decimal(12,2) DEFAULT '0.00',
  `ch_material_flag` varchar(1) DEFAULT NULL,
  `ch_special_type` varchar(1) DEFAULT '0',
  `ch_special_type2` varchar(1) DEFAULT '0',
  `ch_special_type3` varchar(1) DEFAULT '0',
  `ch_special_type4` varchar(1) DEFAULT '0',
  `num_special_price` decimal(12,3) DEFAULT '0.000',
  `vch_dishname_2` varchar(60) DEFAULT NULL,
  `vch_color` varchar(10) DEFAULT NULL,
  `ch_count` varchar(1) DEFAULT 'N',
  `num_count_weight` decimal(12,3) DEFAULT '0.000',
  `ch_qr_code_online` varchar(1) DEFAULT '',
  `int_rate_2` decimal(12,2) DEFAULT '0.00',
  `int_rate_3` decimal(12,2) DEFAULT '0.00',
  `ch_addon_flag` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_dishno`),
  KEY `index_name` (`ch_groupno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_dish`
--

LOCK TABLES `cey_bt_dish` WRITE;
/*!40000 ALTER TABLE `cey_bt_dish` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_dish` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_insert AFTER INSERT ON cey_bt_dish FOR EACH ROW  Begin  delete from del_cey_bt_dish where ch_dishno=NEW.ch_dishno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_update BEFORE UPDATE ON cey_bt_dish FOR EACH ROW  Begin   IF(NEW.ch_dishno = OLD.ch_dishno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_delete BEFORE DELETE ON cey_bt_dish FOR EACH ROW  Begin  replace into del_cey_bt_dish select * from cey_bt_dish where ch_dishno=OLD.ch_dishno; update del_cey_bt_dish set ch_yun_already_keep = 'N' where ch_dishno=OLD.ch_dishno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_dish_subtype`
--

DROP TABLE IF EXISTS `cey_bt_dish_subtype`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_dish_subtype` (
  `ch_sub_typeno` varchar(4) NOT NULL,
  `vch_sub_typename` varchar(20) NOT NULL,
  `ch_dish_typeno` varchar(2) NOT NULL,
  `ch_labelprint_flag` varchar(1) DEFAULT 'N',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `vch_color` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`ch_sub_typeno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_dish_subtype`
--

LOCK TABLES `cey_bt_dish_subtype` WRITE;
/*!40000 ALTER TABLE `cey_bt_dish_subtype` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_dish_subtype` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_subtype_insert AFTER INSERT ON cey_bt_dish_subtype FOR EACH ROW  Begin  delete from del_cey_bt_dish_subtype where ch_sub_typeno=NEW.ch_sub_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_subtype_update BEFORE UPDATE ON cey_bt_dish_subtype FOR EACH ROW  Begin   IF(NEW.ch_sub_typeno = OLD.ch_sub_typeno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_subtype_delete BEFORE DELETE ON cey_bt_dish_subtype FOR EACH ROW  Begin  replace into del_cey_bt_dish_subtype select * from cey_bt_dish_subtype where ch_sub_typeno=OLD.ch_sub_typeno; update del_cey_bt_dish_subtype set ch_yun_already_keep = 'N' where ch_sub_typeno=OLD.ch_sub_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_dish_type`
--

DROP TABLE IF EXISTS `cey_bt_dish_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_dish_type` (
  `ch_dish_typeno` varchar(2) NOT NULL,
  `vch_dish_typename` varchar(20) NOT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `vch_color` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`ch_dish_typeno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_dish_type`
--

LOCK TABLES `cey_bt_dish_type` WRITE;
/*!40000 ALTER TABLE `cey_bt_dish_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_dish_type` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_type_insert AFTER INSERT ON cey_bt_dish_type FOR EACH ROW  Begin  delete from del_cey_bt_dish_type where ch_dish_typeno=NEW.ch_dish_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_type_update BEFORE UPDATE ON cey_bt_dish_type FOR EACH ROW  Begin   IF(NEW.ch_dish_typeno = OLD.ch_dish_typeno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_type_delete BEFORE DELETE ON cey_bt_dish_type FOR EACH ROW  Begin  replace into del_cey_bt_dish_type select * from cey_bt_dish_type where ch_dish_typeno=OLD.ch_dish_typeno; update del_cey_bt_dish_type set ch_yun_already_keep = 'N' where ch_dish_typeno=OLD.ch_dish_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_unit`
--

DROP TABLE IF EXISTS `cey_bt_unit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_unit` (
  `ch_unitno` varchar(2) NOT NULL,
  `vch_unitname` varchar(10) NOT NULL,
  `ch_numtype` varchar(1) DEFAULT '0',
  `num_default` decimal(12,2) DEFAULT '1.00',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_unitno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_unit`
--

LOCK TABLES `cey_bt_unit` WRITE;
/*!40000 ALTER TABLE `cey_bt_unit` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_unit` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_unit_insert AFTER INSERT ON cey_bt_unit FOR EACH ROW  Begin  delete from del_cey_bt_unit where ch_unitno=NEW.ch_unitno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_unit_update BEFORE UPDATE ON cey_bt_unit FOR EACH ROW  Begin   IF(NEW.ch_unitno = OLD.ch_unitno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_unit_delete BEFORE DELETE ON cey_bt_unit FOR EACH ROW  Begin  replace into del_cey_bt_unit select * from cey_bt_unit where ch_unitno=OLD.ch_unitno; update del_cey_bt_unit set ch_yun_already_keep = 'N' where ch_unitno=OLD.ch_unitno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_class`
--

DROP TABLE IF EXISTS `cey_bt_class`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_class` (
  `ch_classno` varchar(2) NOT NULL,
  `vch_classname` varchar(12) NOT NULL,
  `dt_begin` datetime DEFAULT NULL,
  PRIMARY KEY (`ch_classno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_class`
--

LOCK TABLES `cey_bt_class` WRITE;
/*!40000 ALTER TABLE `cey_bt_class` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_class` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_cp_dish`
--

DROP TABLE IF EXISTS `cey_bt_cp_dish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_cp_dish` (
  `ch_dishno` varchar(7) NOT NULL,
  `ch_no` varchar(4) NOT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_dishno`,`ch_no`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_cp_dish`
--

LOCK TABLES `cey_bt_cp_dish` WRITE;
/*!40000 ALTER TABLE `cey_bt_cp_dish` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_cp_dish` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_cp_dish_insert AFTER INSERT ON cey_bt_cp_dish FOR EACH ROW  Begin  delete from del_cey_bt_cp_dish where ch_dishno=NEW.ch_dishno and ch_no=NEW.ch_no; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_cp_dish_update BEFORE UPDATE ON cey_bt_cp_dish FOR EACH ROW  Begin   IF(NEW.ch_dishno = OLD.ch_dishno and NEW.ch_no = OLD.ch_no and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_cp_dish_delete BEFORE DELETE ON cey_bt_cp_dish FOR EACH ROW  Begin  replace into del_cey_bt_cp_dish select * from cey_bt_cp_dish where ch_dishno=OLD.ch_dishno and ch_no=OLD.ch_no; update del_cey_bt_cp_dish set ch_yun_already_keep = 'N' where ch_dishno=OLD.ch_dishno and ch_no=OLD.ch_no; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_cp_memo`
--

DROP TABLE IF EXISTS `cey_bt_cp_memo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_cp_memo` (
  `ch_no` varchar(4) NOT NULL,
  `vch_cp_memo` varchar(40) NOT NULL,
  `ch_typeno` varchar(2) NOT NULL,
  `num_add_price` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_no`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_cp_memo`
--

LOCK TABLES `cey_bt_cp_memo` WRITE;
/*!40000 ALTER TABLE `cey_bt_cp_memo` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_cp_memo` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_cp_memo_insert AFTER INSERT ON cey_bt_cp_memo FOR EACH ROW  Begin  delete from del_cey_bt_cp_memo where ch_no=NEW.ch_no; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_cp_memo_update BEFORE UPDATE ON cey_bt_cp_memo FOR EACH ROW  Begin   IF(NEW.ch_no = OLD.ch_no and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_cp_memo_delete BEFORE DELETE ON cey_bt_cp_memo FOR EACH ROW  Begin  replace into del_cey_bt_cp_memo select * from cey_bt_cp_memo where ch_no=OLD.ch_no; update del_cey_bt_cp_memo set ch_yun_already_keep = 'N' where ch_no=OLD.ch_no; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_cp_type`
--

DROP TABLE IF EXISTS `cey_bt_cp_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_cp_type` (
  `ch_typeno` varchar(2) NOT NULL,
  `vch_typename` varchar(20) NOT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_typeno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_cp_type`
--

LOCK TABLES `cey_bt_cp_type` WRITE;
/*!40000 ALTER TABLE `cey_bt_cp_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_cp_type` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_cp_type_insert AFTER INSERT ON cey_bt_cp_type FOR EACH ROW  Begin  delete from del_cey_bt_cp_type where ch_typeno=NEW.ch_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_cp_type_update BEFORE UPDATE ON cey_bt_cp_type FOR EACH ROW  Begin   IF(NEW.ch_typeno = OLD.ch_typeno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_cp_type_delete BEFORE DELETE ON cey_bt_cp_type FOR EACH ROW  Begin  replace into del_cey_bt_cp_type select * from cey_bt_cp_type where ch_typeno=OLD.ch_typeno; update del_cey_bt_cp_type set ch_yun_already_keep = 'N' where ch_typeno=OLD.ch_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_dish_del`
--

DROP TABLE IF EXISTS `cey_bt_dish_del`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_dish_del` (
  `ch_dishno` varchar(7) NOT NULL,
  `vch_dishname` varchar(60) NOT NULL,
  `vch_spell` varchar(40) DEFAULT NULL,
  `vch_english` varchar(40) DEFAULT NULL,
  `ch_dish_typeno` varchar(2) DEFAULT NULL,
  `ch_unitno` varchar(2) DEFAULT NULL,
  `num_price` decimal(12,2) DEFAULT '0.00',
  `ch_suitflag` varchar(1) DEFAULT 'N',
  `ch_discount` varchar(1) DEFAULT 'N',
  `ch_curprice` varchar(1) DEFAULT 'N',
  `ch_tempflag` varchar(1) DEFAULT 'N',
  `ch_stopflag` varchar(1) DEFAULT NULL,
  `ch_printflag` varchar(1) DEFAULT NULL,
  `vch_printer` varchar(50) DEFAULT NULL,
  `ch_sub_typeno` varchar(4) DEFAULT NULL,
  `ch_materialno` varchar(7) DEFAULT NULL,
  `ch_serviceflag` varchar(1) DEFAULT NULL,
  `ch_lowflag` varchar(1) DEFAULT NULL,
  `num_m_price` decimal(12,2) DEFAULT '0.00',
  `ch_deductflag` varchar(1) DEFAULT NULL,
  `ch_deducttype` varchar(1) DEFAULT NULL,
  `int_deduct` int(11) DEFAULT NULL,
  `num_deduct` decimal(12,2) DEFAULT NULL,
  `ch_saleflag` varchar(1) DEFAULT NULL,
  `ch_saletype` varchar(1) DEFAULT NULL,
  `int_sale_deduct` int(11) DEFAULT NULL,
  `num_sale` decimal(12,2) DEFAULT NULL,
  `item_flag` varchar(1) DEFAULT NULL,
  `vch_pur_unitno` varchar(2) DEFAULT NULL,
  `int_unit_rate` int(11) DEFAULT NULL,
  `ch_off` varchar(1) DEFAULT 'N',
  `vch_subno` varchar(40) DEFAULT NULL,
  `num_sale_price` decimal(20,2) DEFAULT '0.00',
  `ch_outflag` varchar(1) DEFAULT NULL,
  `vch_outprint` varchar(50) DEFAULT NULL,
  `int_rate` decimal(12,2) DEFAULT NULL,
  `ch_weight` varchar(1) DEFAULT NULL,
  `vch_barcode` varchar(64) DEFAULT NULL,
  `ch_kitchen_print_flag` varchar(1) DEFAULT NULL,
  `vch_kitchen_print_id1` varchar(2) DEFAULT NULL,
  `vch_kitchen_print_id2` varchar(2) DEFAULT NULL,
  `vch_kitchen_print_id3` varchar(2) DEFAULT NULL,
  `ch_kitchen_out_flag` varchar(1) DEFAULT NULL,
  `vch_kitchen_out_id1` varchar(2) DEFAULT NULL,
  `vch_kitchen_out_id2` varchar(2) DEFAULT NULL,
  `vch_kitchen_out_id3` varchar(2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `ch_groupno` varchar(7) DEFAULT NULL,
  PRIMARY KEY (`ch_dishno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_dish_del`
--

LOCK TABLES `cey_bt_dish_del` WRITE;
/*!40000 ALTER TABLE `cey_bt_dish_del` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_dish_del` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_dish_item`
--

DROP TABLE IF EXISTS `cey_bt_dish_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_dish_item` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT COMMENT '流水号',
  `ch_dishno` varchar(7) NOT NULL COMMENT '菜品编码',
  `ch_itemno` varchar(7) NOT NULL COMMENT '原料编码',
  `num_num` decimal(12,0) DEFAULT '0' COMMENT '原料数量',
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_dish_item`
--

LOCK TABLES `cey_bt_dish_item` WRITE;
/*!40000 ALTER TABLE `cey_bt_dish_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_dish_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_dish_special`
--

DROP TABLE IF EXISTS `cey_bt_dish_special`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_dish_special` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `dt_date` datetime NOT NULL,
  `ch_dishno` varchar(7) DEFAULT NULL,
  `num_price` decimal(12,2) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_operID2` varchar(6) DEFAULT NULL,
  `dt_operdate2` datetime DEFAULT NULL,
  `dt_operdate_effect` datetime DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_dish_special`
--

LOCK TABLES `cey_bt_dish_special` WRITE;
/*!40000 ALTER TABLE `cey_bt_dish_special` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_dish_special` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_dish_suit`
--

DROP TABLE IF EXISTS `cey_bt_dish_suit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_dish_suit` (
  `int_flowid` int(11) NOT NULL AUTO_INCREMENT,
  `ch_suitno` varchar(7) NOT NULL,
  `ch_dishno` varchar(7) DEFAULT NULL,
  `num_num` decimal(12,2) DEFAULT '1.00',
  `ch_dishno_chang` varchar(7) DEFAULT NULL,
  `num_num_change` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`int_flowid`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_dish_suit`
--

LOCK TABLES `cey_bt_dish_suit` WRITE;
/*!40000 ALTER TABLE `cey_bt_dish_suit` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_dish_suit` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_suit_insert AFTER INSERT ON cey_bt_dish_suit FOR EACH ROW  Begin  delete from del_cey_bt_dish_suit where int_flowid=NEW.int_flowid; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_suit_update BEFORE UPDATE ON cey_bt_dish_suit FOR EACH ROW  Begin   IF(NEW.int_flowid = OLD.int_flowid and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_suit_delete BEFORE DELETE ON cey_bt_dish_suit FOR EACH ROW  Begin  replace into del_cey_bt_dish_suit select * from cey_bt_dish_suit where int_flowid=OLD.int_flowid; update del_cey_bt_dish_suit set ch_yun_already_keep = 'N' where int_flowid=OLD.int_flowid; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_dish_type_oper`
--

DROP TABLE IF EXISTS `cey_bt_dish_type_oper`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_dish_type_oper` (
  `vch_operid` varchar(10) NOT NULL,
  `ch_dish_typeno` varchar(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_dish_type_oper`
--

LOCK TABLES `cey_bt_dish_type_oper` WRITE;
/*!40000 ALTER TABLE `cey_bt_dish_type_oper` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_dish_type_oper` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_dish_warn`
--

DROP TABLE IF EXISTS `cey_bt_dish_warn`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_dish_warn` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `dt_date` datetime NOT NULL,
  `ch_dishno` varchar(7) DEFAULT NULL,
  `num_num` decimal(12,2) DEFAULT NULL,
  `num_sale` decimal(12,2) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_operID2` varchar(6) DEFAULT NULL,
  `dt_operdate2` datetime DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_dish_warn`
--

LOCK TABLES `cey_bt_dish_warn` WRITE;
/*!40000 ALTER TABLE `cey_bt_dish_warn` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_dish_warn` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_warn_insert AFTER INSERT ON cey_bt_dish_warn FOR EACH ROW  Begin  delete from del_cey_bt_dish_warn where int_flowid=NEW.int_flowid; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_warn_update BEFORE UPDATE ON cey_bt_dish_warn FOR EACH ROW  Begin   IF(NEW.int_flowid = OLD.int_flowid and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_dish_warn_delete BEFORE DELETE ON cey_bt_dish_warn FOR EACH ROW  Begin  replace into del_cey_bt_dish_warn select * from cey_bt_dish_warn where int_flowid=OLD.int_flowid; update del_cey_bt_dish_warn set ch_yun_already_keep = 'N' where int_flowid=OLD.int_flowid; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_kitchen_plan`
--

DROP TABLE IF EXISTS `cey_bt_kitchen_plan`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_kitchen_plan` (
  `vch_plan_id` varchar(2) NOT NULL,
  `vch_plan_name` varchar(32) DEFAULT NULL,
  `int_print_id` int(2) DEFAULT NULL,
  `int_print_id_second` int(2) DEFAULT NULL,
  `int_suit_type` int(2) DEFAULT NULL,
  `int_dish_type` int(2) DEFAULT NULL,
  `int_print_count` int(2) DEFAULT NULL,
  `int_tail_block` int(2) DEFAULT NULL,
  `vch_plan_title` varchar(100) DEFAULT NULL,
  `vch_plan_tail` varchar(100) DEFAULT NULL,
  `int_head_block` int(2) DEFAULT NULL,
  `ch_check_state` varchar(1) DEFAULT NULL,
  `ch_print_id_second_use` varchar(1) DEFAULT NULL,
  `vch_dishname_No` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`vch_plan_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_kitchen_plan`
--

LOCK TABLES `cey_bt_kitchen_plan` WRITE;
/*!40000 ALTER TABLE `cey_bt_kitchen_plan` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_kitchen_plan` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_kitchen_print`
--

DROP TABLE IF EXISTS `cey_bt_kitchen_print`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_kitchen_print` (
  `int_id` int(32) NOT NULL,
  `vch_printname` varchar(32) DEFAULT NULL,
  `vch_printip` varchar(32) DEFAULT NULL,
  `vch_printtype` varchar(32) DEFAULT NULL,
  `vch_instype` varchar(32) DEFAULT NULL,
  `int_paperwidth` int(4) DEFAULT NULL,
  `ch_areaNo` varchar(2) DEFAULT NULL,
  `int_xinye_beep_count` int(11) DEFAULT '4',
  `ch_kitchen_view` varchar(1) DEFAULT 'N',
  PRIMARY KEY (`int_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_kitchen_print`
--

LOCK TABLES `cey_bt_kitchen_print` WRITE;
/*!40000 ALTER TABLE `cey_bt_kitchen_print` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_kitchen_print` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_kitchen_view`
--

DROP TABLE IF EXISTS `cey_bt_kitchen_view`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_kitchen_view` (
  `vch_plan_id` varchar(2) NOT NULL,
  `vch_plan_name` varchar(32) DEFAULT NULL,
  `vch_plan_id_service` varchar(2) DEFAULT NULL,
  `vch_kitchen_type` varchar(32) DEFAULT NULL,
  `vch_shop_type` varchar(32) DEFAULT NULL,
  `ch_pop_new_bill_info` varchar(1) DEFAULT NULL,
  `ch_auto_take` varchar(1) DEFAULT NULL,
  `int_auto_take` int(11) DEFAULT NULL,
  `int_order_warn_mins` int(11) DEFAULT NULL,
  `ch_voice_count` varchar(1) DEFAULT NULL,
  `int_voice_count` int(11) DEFAULT NULL,
  `vch_language` varchar(8) DEFAULT NULL,
  `vch_screen_primary_size` varchar(32) DEFAULT NULL,
  `ch_screen_secondary` varchar(1) DEFAULT NULL,
  `vch_screen_secondary_size` varchar(32) DEFAULT NULL,
  `vch_terminal` varchar(64) NOT NULL,
  `ch_refresh_order_insert` varchar(1) DEFAULT 'N',
  `ch_refresh_order_update` varchar(1) DEFAULT 'N',
  `ch_refresh_order_quit` varchar(1) DEFAULT 'N',
  `ch_refresh_sys_geometry` varchar(1) DEFAULT 'N',
  `ch_auto_login` varchar(1) DEFAULT 'N',
  `vch_operID` varchar(6) DEFAULT NULL,
  PRIMARY KEY (`vch_plan_id`),
  UNIQUE KEY `vch_terminal` (`vch_terminal`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_kitchen_view`
--

LOCK TABLES `cey_bt_kitchen_view` WRITE;
/*!40000 ALTER TABLE `cey_bt_kitchen_view` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_kitchen_view` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_paymode`
--

DROP TABLE IF EXISTS `cey_bt_paymode`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_paymode` (
  `ch_paymodeno` varchar(2) NOT NULL,
  `vch_paymodename` varchar(60) NOT NULL,
  `ch_IDflag` varchar(1) DEFAULT NULL,
  `ch_faceflag` varchar(1) DEFAULT NULL,
  `ch_changeflag` varchar(1) DEFAULT NULL,
  `ch_incomeflag` varchar(1) DEFAULT NULL,
  `ch_key` varchar(1) DEFAULT NULL,
  `vch_arg1` varchar(128) DEFAULT NULL,
  `vch_arg2` varchar(128) DEFAULT NULL,
  `vch_arg3` text,
  `vch_arg4` varchar(128) DEFAULT NULL,
  `vch_arg5` varchar(128) DEFAULT NULL,
  `vch_arg6` varchar(128) DEFAULT NULL,
  `vch_arg7` varchar(128) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `vch_english` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`ch_paymodeno`),
  UNIQUE KEY `vch_english` (`vch_english`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_paymode`
--

LOCK TABLES `cey_bt_paymode` WRITE;
/*!40000 ALTER TABLE `cey_bt_paymode` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_paymode` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_paymode_insert AFTER INSERT ON cey_bt_paymode FOR EACH ROW  Begin  delete from del_cey_bt_paymode where ch_paymodeno=NEW.ch_paymodeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_paymode_update BEFORE UPDATE ON cey_bt_paymode FOR EACH ROW  Begin   IF(NEW.ch_paymodeno = OLD.ch_paymodeno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_paymode_delete BEFORE DELETE ON cey_bt_paymode FOR EACH ROW  Begin  replace into del_cey_bt_paymode select * from cey_bt_paymode where ch_paymodeno=OLD.ch_paymodeno; update del_cey_bt_paymode set ch_yun_already_keep = 'N' where ch_paymodeno=OLD.ch_paymodeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_project`
--

DROP TABLE IF EXISTS `cey_bt_project`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_project` (
  `ch_projectno` varchar(3) NOT NULL,
  `vch_projectname` varchar(30) NOT NULL,
  `ch_dishflag` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_projectno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_project`
--

LOCK TABLES `cey_bt_project` WRITE;
/*!40000 ALTER TABLE `cey_bt_project` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_project` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_project_detail`
--

DROP TABLE IF EXISTS `cey_bt_project_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_project_detail` (
  `ch_projectno` varchar(3) NOT NULL,
  `ch_typeno` varchar(4) NOT NULL,
  `int_discount` int(11) NOT NULL,
  PRIMARY KEY (`ch_projectno`,`ch_typeno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_project_detail`
--

LOCK TABLES `cey_bt_project_detail` WRITE;
/*!40000 ALTER TABLE `cey_bt_project_detail` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_project_detail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_reason`
--

DROP TABLE IF EXISTS `cey_bt_reason`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_reason` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `vch_reason` varchar(50) NOT NULL,
  `vch_type` varchar(20) NOT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_reason`
--

LOCK TABLES `cey_bt_reason` WRITE;
/*!40000 ALTER TABLE `cey_bt_reason` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_reason` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_suit_change`
--

DROP TABLE IF EXISTS `cey_bt_suit_change`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_suit_change` (
  `int_flowID` int(11) NOT NULL,
  `ch_dishno` varchar(7) NOT NULL,
  `num_num_change` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_dishno`,`int_flowID`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_suit_change`
--

LOCK TABLES `cey_bt_suit_change` WRITE;
/*!40000 ALTER TABLE `cey_bt_suit_change` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_suit_change` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_suit_change_insert AFTER INSERT ON cey_bt_suit_change FOR EACH ROW  Begin  delete from del_cey_bt_suit_change where int_flowid=NEW.int_flowid and ch_dishno=NEW.ch_dishno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_suit_change_update BEFORE UPDATE ON cey_bt_suit_change FOR EACH ROW  Begin   IF(NEW.int_flowid = OLD.int_flowid and NEW.ch_dishno = OLD.ch_dishno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_suit_change_delete BEFORE DELETE ON cey_bt_suit_change FOR EACH ROW  Begin  replace into del_cey_bt_suit_change select * from cey_bt_suit_change where int_flowid=OLD.int_flowid and ch_dishno=OLD.ch_dishno; update del_cey_bt_suit_change set ch_yun_already_keep = 'N' where int_flowid=OLD.int_flowid and ch_dishno=OLD.ch_dishno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_table`
--

DROP TABLE IF EXISTS `cey_bt_table`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_table` (
  `ch_tableno` varchar(4) NOT NULL,
  `vch_tablename` varchar(12) NOT NULL,
  `ch_typeno` varchar(2) DEFAULT NULL,
  `ch_billno` varchar(15) DEFAULT NULL,
  `ch_lockflag` varchar(1) DEFAULT NULL,
  `int_person` int(11) DEFAULT NULL,
  `vch_memo` varchar(100) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `dt_datetime` datetime DEFAULT NULL,
  `ch_areaNo` varchar(2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `ch_billno_1` varchar(15) DEFAULT NULL,
  `ch_billno_2` varchar(15) DEFAULT NULL,
  `ch_billno_3` varchar(15) DEFAULT NULL,
  `ch_billno_4` varchar(15) DEFAULT NULL,
  `ch_billno_5` varchar(15) DEFAULT NULL,
  `ch_billno_6` varchar(15) DEFAULT NULL,
  `ch_billno_7` varchar(15) DEFAULT NULL,
  `ch_billno_8` varchar(15) DEFAULT NULL,
  `ch_lockflag_1` varchar(1) DEFAULT NULL,
  `ch_lockflag_2` varchar(1) DEFAULT NULL,
  `ch_lockflag_3` varchar(1) DEFAULT NULL,
  `ch_lockflag_4` varchar(1) DEFAULT NULL,
  `ch_lockflag_5` varchar(1) DEFAULT NULL,
  `ch_lockflag_6` varchar(1) DEFAULT NULL,
  `ch_lockflag_7` varchar(1) DEFAULT NULL,
  `ch_lockflag_8` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_tableno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_table`
--

LOCK TABLES `cey_bt_table` WRITE;
/*!40000 ALTER TABLE `cey_bt_table` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_table` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_table_insert AFTER INSERT ON cey_bt_table FOR EACH ROW  Begin  delete from del_cey_bt_table where ch_tableno=NEW.ch_tableno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_table_update BEFORE UPDATE ON cey_bt_table FOR EACH ROW  Begin   IF(NEW.ch_tableno = OLD.ch_tableno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_table_delete BEFORE DELETE ON cey_bt_table FOR EACH ROW  Begin  replace into del_cey_bt_table select * from cey_bt_table where ch_tableno=OLD.ch_tableno; update del_cey_bt_table set ch_yun_already_keep = 'N' where ch_tableno=OLD.ch_tableno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_table_area`
--

DROP TABLE IF EXISTS `cey_bt_table_area`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_table_area` (
  `ch_areano` varchar(2) NOT NULL,
  `vch_areaname` varchar(12) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_areano`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_table_area`
--

LOCK TABLES `cey_bt_table_area` WRITE;
/*!40000 ALTER TABLE `cey_bt_table_area` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_table_area` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_table_area_insert AFTER INSERT ON cey_bt_table_area FOR EACH ROW  Begin  delete from del_cey_bt_table_area where ch_areano=NEW.ch_areano; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_table_area_update BEFORE UPDATE ON cey_bt_table_area FOR EACH ROW  Begin   IF(NEW.ch_areano = OLD.ch_areano and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_table_area_delete BEFORE DELETE ON cey_bt_table_area FOR EACH ROW  Begin  replace into del_cey_bt_table_area select * from cey_bt_table_area where ch_areano=OLD.ch_areano; update del_cey_bt_table_area set ch_yun_already_keep = 'N' where ch_areano=OLD.ch_areano; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_table_bar`
--

DROP TABLE IF EXISTS `cey_bt_table_bar`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_table_bar` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `ch_tableno` varchar(4) NOT NULL,
  `int_div_id` int(4) NOT NULL,
  `ch_billno` varchar(15) NOT NULL,
  PRIMARY KEY (`int_flowID`),
  UNIQUE KEY `Idx_cey_u_orderdish` (`ch_tableno`,`int_div_id`,`ch_billno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_table_bar`
--

LOCK TABLES `cey_bt_table_bar` WRITE;
/*!40000 ALTER TABLE `cey_bt_table_bar` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_table_bar` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_table_bar_wine`
--

DROP TABLE IF EXISTS `cey_bt_table_bar_wine`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_table_bar_wine` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `ch_dishno` varchar(7) NOT NULL,
  `num_dish` decimal(12,2) NOT NULL,
  `vch_handle` varchar(20) NOT NULL,
  `vch_guest` varchar(50) NOT NULL,
  `vch_operID` varchar(6) NOT NULL,
  `dt_operdate` datetime NOT NULL,
  `dt_enddate` date NOT NULL,
  `vch_memo` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`),
  KEY `idx_cey_bt_table_bar_wine` (`vch_handle`),
  KEY `idx_cey_bt_table_bar_wine_2` (`vch_guest`),
  KEY `idx_cey_bt_table_bar_wine_3` (`vch_operID`),
  KEY `idx_cey_bt_table_bar_wine_4` (`dt_operdate`),
  KEY `idx_cey_bt_table_bar_wine_5` (`dt_enddate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_table_bar_wine`
--

LOCK TABLES `cey_bt_table_bar_wine` WRITE;
/*!40000 ALTER TABLE `cey_bt_table_bar_wine` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_table_bar_wine` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_table_type`
--

DROP TABLE IF EXISTS `cey_bt_table_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_table_type` (
  `ch_typeno` varchar(2) NOT NULL,
  `vch_typename` varchar(12) NOT NULL,
  `ch_pricekind` varchar(1) DEFAULT NULL,
  `ch_service_mode` varchar(1) DEFAULT NULL,
  `ch_serviceflag` varchar(1) DEFAULT NULL,
  `int_start_minute` int(11) DEFAULT NULL,
  `num_start_amount` decimal(12,2) DEFAULT NULL,
  `int_unit_minute` int(11) DEFAULT NULL,
  `num_unit_amount` decimal(12,2) DEFAULT NULL,
  `int_over` int(11) DEFAULT NULL,
  `num_ration` decimal(12,2) DEFAULT NULL,
  `int_rate` int(11) DEFAULT NULL,
  `num_max_service` decimal(12,2) DEFAULT NULL,
  `ch_low_mode` varchar(1) DEFAULT NULL,
  `num_lowcost` decimal(12,2) DEFAULT NULL,
  `int_begin_time` int(11) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_typeno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_table_type`
--

LOCK TABLES `cey_bt_table_type` WRITE;
/*!40000 ALTER TABLE `cey_bt_table_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_table_type` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_table_type_insert AFTER INSERT ON cey_bt_table_type FOR EACH ROW  Begin  delete from del_cey_bt_table_type where ch_typeno=NEW.ch_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_table_type_update BEFORE UPDATE ON cey_bt_table_type FOR EACH ROW  Begin   IF(NEW.ch_typeno = OLD.ch_typeno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_bt_table_type_delete BEFORE DELETE ON cey_bt_table_type FOR EACH ROW  Begin  replace into del_cey_bt_table_type select * from cey_bt_table_type where ch_typeno=OLD.ch_typeno; update del_cey_bt_table_type set ch_yun_already_keep = 'N' where ch_typeno=OLD.ch_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_bt_yun_table_delrecord`
--

DROP TABLE IF EXISTS `cey_bt_yun_table_delrecord`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_yun_table_delrecord` (
  `vch_sqltablename` varchar(20) NOT NULL,
  `vch_value` varchar(50) NOT NULL,
  `vch_keyname` varchar(20) NOT NULL,
  UNIQUE KEY `vch_sqltablename` (`vch_sqltablename`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_yun_table_delrecord`
--

LOCK TABLES `cey_bt_yun_table_delrecord` WRITE;
/*!40000 ALTER TABLE `cey_bt_yun_table_delrecord` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_bt_yun_table_delrecord` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_bt_yun_table_version`
--

DROP TABLE IF EXISTS `cey_bt_yun_table_version`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_bt_yun_table_version` (
  `vch_sqltablename` varchar(20) NOT NULL,
  `vch_version` int(10) NOT NULL,
  UNIQUE KEY `vch_sqltablename` (`vch_sqltablename`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_bt_yun_table_version`
--

LOCK TABLES `cey_bt_yun_table_version` WRITE;
/*!40000 ALTER TABLE `cey_bt_yun_table_version` DISABLE KEYS */;
INSERT INTO `cey_bt_yun_table_version` VALUES ('cey_bt_class',0),('cey_bt_cp_dish',0),('cey_bt_cp_memo',0),('cey_bt_cp_type',0),('cey_bt_dish',0),('cey_bt_dish_subtype',0),('cey_bt_dish_suit',0),('cey_bt_dish_type',0),('cey_bt_dish_warn',0),('cey_bt_paymode',0),('cey_bt_suit_change',0),('cey_bt_table',0),('cey_bt_table_area',0),('cey_bt_table_type',0),('cey_bt_unit',0),('cey_sys_company',0),('cey_u_checkout_detai',0),('cey_u_checkout_maste',0),('cey_u_day_sumup',0),('cey_u_master',0),('cey_u_orderdish',0),('cey_u_orderdish_log',0),('cey_u_service_low',0),('cey_u_togo',0),('t_m_curamount',0),('t_m_deposit',0),('t_m_gift',0),('t_m_member',0),('t_m_member_type',0),('t_m_parameter',0),('t_m_pay',0),('t_m_point',0);
/*!40000 ALTER TABLE `cey_bt_yun_table_version` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_st_check_detail`
--

DROP TABLE IF EXISTS `cey_st_check_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_st_check_detail` (
  `ch_sheetno` varchar(12) NOT NULL,
  `ch_materialno` varchar(7) NOT NULL,
  `num_sys` decimal(18,4) DEFAULT '0.0000',
  `num_check` decimal(18,4) DEFAULT '0.0000',
  `num_pro_loss` decimal(18,4) DEFAULT '0.0000',
  `cost_price` decimal(18,4) DEFAULT '0.0000',
  `chg_flag` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_materialno`,`ch_sheetno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_st_check_detail`
--

LOCK TABLES `cey_st_check_detail` WRITE;
/*!40000 ALTER TABLE `cey_st_check_detail` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_st_check_detail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_st_check_master`
--

DROP TABLE IF EXISTS `cey_st_check_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_st_check_master` (
  `ch_sheetno` varchar(12) NOT NULL,
  `vch_handle` varchar(6) DEFAULT NULL,
  `vch_operId` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_audit` varchar(6) DEFAULT NULL,
  `dt_audit` datetime DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`ch_sheetno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_st_check_master`
--

LOCK TABLES `cey_st_check_master` WRITE;
/*!40000 ALTER TABLE `cey_st_check_master` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_st_check_master` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER `tr_st_check_master_update` AFTER UPDATE ON `cey_st_check_master` FOR EACH ROW BEGIN
   DECLARE m_done int default 0;
   DECLARE sheetNo  varchar(12);
   DECLARE materialno varchar(7);
   DECLARE numnum numeric(16,4); 
   DECLARE intflowId integer;
   DECLARE initqty   numeric(16,4);
   DECLARE initprice numeric(16,4);
   DECLARE remainqty numeric(16,4);
   DECLARE remainprice numeric(16,4) ;
   DECLARE cur_rows cursor for
     select b.ch_materialno, b.num_pro_loss from  cey_st_check_detail b 
       where NEW.ch_sheetno = b.ch_sheetno and b.num_pro_loss <> 0
         and b.chg_flag = 'Y'
       order by b.ch_sheetno, b.ch_materialno; 

  DECLARE continue handler for not found set m_done = 1;

  open cur_rows;

  WHILE (m_done = 0) DO 

    fetch cur_rows into materialno, numnum;

     IF(m_done = 0) THEN

      IF(NEW.ch_state = 'Y')THEN

       set sheetNo = NEW.ch_sheetno;

       set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow where ch_materialno = materialno);

        IF (intflowId = 0) THEN
           set initqty = 0;
        ELSE
           set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);
        END IF;

       IF (intflowId = 0) THEN
           set initprice = 0;
       ELSE
           set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);
       END IF;
        
       set remainqty = initqty + numnum;
       set remainprice = initprice;
        
       IF (numnum > 0) THEN
           insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
                  num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)
            values (materialno, now(), 'PD', 1, numnum, 0, remainprice, 0, remainqty, remainprice, sheetNo);
       ELSEIF (numnum < 0) THEN
           insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
                  num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)
            values (materialno, now(), 'PD', -1, abs(numnum), 0, remainprice, 0, remainqty, remainprice, sheetNo);
       END IF;
        
       IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN
             update cey_st_material_num set num_num = remainqty, cost_price = remainprice
              where ch_materialno = materialno;
       ELSE
             insert into cey_st_material_num (ch_materialno, num_num, cost_price)
               values (materialno, remainqty, remainprice);
       END IF;
      
      END IF;
        
     END IF;

   END WHILE;

   close cur_rows;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_st_material_flow`
--

DROP TABLE IF EXISTS `cey_st_material_flow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_st_material_flow` (
  `int_flowId` int(11) NOT NULL AUTO_INCREMENT,
  `ch_materialno` varchar(7) NOT NULL,
  `dt_operdate` datetime NOT NULL,
  `sheettype` varchar(3) NOT NULL,
  `int_flag` int(11) NOT NULL,
  `num_num` decimal(18,4) DEFAULT '0.0000',
  `sheet_price` decimal(18,4) DEFAULT '0.0000',
  `cost_price` decimal(18,4) DEFAULT '0.0000',
  `chg_amount` decimal(18,4) DEFAULT '0.0000',
  `remain_qty` decimal(18,4) DEFAULT '0.0000',
  `remain_price` decimal(18,4) DEFAULT '0.0000',
  `ch_sheetno` varchar(12) DEFAULT NULL,
  PRIMARY KEY (`int_flowId`),
  KEY `Index_1` (`ch_materialno`,`dt_operdate`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_st_material_flow`
--

LOCK TABLES `cey_st_material_flow` WRITE;
/*!40000 ALTER TABLE `cey_st_material_flow` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_st_material_flow` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_st_material_num`
--

DROP TABLE IF EXISTS `cey_st_material_num`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_st_material_num` (
  `ch_materialno` varchar(7) NOT NULL,
  `num_num` decimal(18,4) DEFAULT '0.0000',
  `cost_price` decimal(18,4) DEFAULT '0.0000',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_materialno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_st_material_num`
--

LOCK TABLES `cey_st_material_num` WRITE;
/*!40000 ALTER TABLE `cey_st_material_num` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_st_material_num` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'IGNORE_SPACE' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`127.0.0.1`*/ /*!50003 TRIGGER tr_web_cey_st_material_num_update BEFORE UPDATE ON cey_st_material_num FOR EACH ROW  Begin   IF(NEW.ch_materialno = OLD.ch_materialno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_st_provider`
--

DROP TABLE IF EXISTS `cey_st_provider`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_st_provider` (
  `ch_providerNo` varchar(8) NOT NULL,
  `vch_providerName` varchar(60) NOT NULL,
  `vch_tax_number` varchar(60) DEFAULT NULL,
  `ch_typeno` varchar(15) DEFAULT NULL,
  `vch_linkman` varchar(60) DEFAULT NULL,
  `vch_tel` varchar(16) DEFAULT NULL,
  `vch_email` varchar(60) DEFAULT NULL,
  `vch_adress` varchar(60) DEFAULT NULL,
  `vch_voucherno` varchar(60) DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `int_tax` decimal(12,2) DEFAULT '0.00',
  `num_remain` decimal(12,2) DEFAULT '0.00',
  PRIMARY KEY (`ch_providerNo`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_st_provider`
--

LOCK TABLES `cey_st_provider` WRITE;
/*!40000 ALTER TABLE `cey_st_provider` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_st_provider` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_st_provider_pay`
--

DROP TABLE IF EXISTS `cey_st_provider_pay`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_st_provider_pay` (
  `ch_sheetno` varchar(12) NOT NULL,
  `ch_sheetType` varchar(2) NOT NULL,
  `ch_providerNo` varchar(8) DEFAULT NULL,
  `num_cost` decimal(12,2) DEFAULT '0.00',
  `num_tax` decimal(12,2) DEFAULT '0.00',
  `num_cash` decimal(12,2) DEFAULT '0.00',
  `num_remain` decimal(12,2) DEFAULT '0.00',
  `dt_operdate` datetime DEFAULT NULL,
  PRIMARY KEY (`ch_sheetno`),
  KEY `idx_cey_st_provider_pay1` (`ch_providerNo`),
  KEY `idx_cey_st_provider_pay2` (`dt_operdate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_st_provider_pay`
--

LOCK TABLES `cey_st_provider_pay` WRITE;
/*!40000 ALTER TABLE `cey_st_provider_pay` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_st_provider_pay` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_st_provider_pay_flow`
--

DROP TABLE IF EXISTS `cey_st_provider_pay_flow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_st_provider_pay_flow` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `ch_sheetno` varchar(12) NOT NULL,
  `ch_providerNo` varchar(8) DEFAULT NULL,
  `num_pay` decimal(12,2) DEFAULT '0.00',
  `num_remain` decimal(12,2) DEFAULT '0.00',
  `dt_operdate` datetime DEFAULT NULL,
  PRIMARY KEY (`int_flowID`),
  KEY `idx_cey_st_provider_flow1` (`ch_sheetno`),
  KEY `idx_cey_st_provider_flow2` (`ch_providerNo`),
  KEY `idx_cey_st_provider_flow3` (`dt_operdate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_st_provider_pay_flow`
--

LOCK TABLES `cey_st_provider_pay_flow` WRITE;
/*!40000 ALTER TABLE `cey_st_provider_pay_flow` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_st_provider_pay_flow` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_st_sheet_detail`
--

DROP TABLE IF EXISTS `cey_st_sheet_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_st_sheet_detail` (
  `ch_sheetno` varchar(12) NOT NULL,
  `int_id` int(11) NOT NULL,
  `ch_materialno` varchar(7) DEFAULT NULL,
  `num_num` decimal(18,4) DEFAULT '0.0000',
  `num_wrapping` decimal(18,4) DEFAULT '0.0000',
  `num_present` decimal(17,4) DEFAULT '0.0000',
  `num_price` decimal(18,4) DEFAULT '0.0000',
  `cost_price` decimal(18,4) DEFAULT '0.0000',
  `num_already` decimal(18,4) DEFAULT '0.0000',
  `vch_memo` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`ch_sheetno`,`int_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_st_sheet_detail`
--

LOCK TABLES `cey_st_sheet_detail` WRITE;
/*!40000 ALTER TABLE `cey_st_sheet_detail` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_st_sheet_detail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_st_sheet_master`
--

DROP TABLE IF EXISTS `cey_st_sheet_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_st_sheet_master` (
  `ch_sheetNo` varchar(12) NOT NULL,
  `ch_sheetType` varchar(2) NOT NULL,
  `ch_operation` varchar(3) DEFAULT NULL,
  `ch_providerNo` varchar(8) DEFAULT NULL,
  `vch_handle` varchar(6) DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `vch_operId` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_audit` varchar(6) DEFAULT NULL,
  `dt_audit` datetime DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `int_tax` decimal(12,2) DEFAULT '0.00',
  PRIMARY KEY (`ch_sheetNo`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_st_sheet_master`
--

LOCK TABLES `cey_st_sheet_master` WRITE;
/*!40000 ALTER TABLE `cey_st_sheet_master` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_st_sheet_master` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER tr_st_sheet_master_update AFTER UPDATE ON cey_st_sheet_master FOR EACH ROW 
    BEGIN
        DECLARE m_de int default 0;
        DECLARE m_done int default 0;
        DECLARE sheetNo  varchar(12);
        DECLARE sheetType varchar(2);
        DECLARE operation varchar(3);
        DECLARE dtaudit datetime;
        DECLARE materialno varchar(7);
        DECLARE numnum numeric(16,4);
        DECLARE numpresent numeric(15,4);
        DECLARE numprice numeric(16,4);
        DECLARE costprice  numeric(16,4);
        DECLARE unitrate numeric(16,2);
        DECLARE intflowId integer;
        DECLARE initqty   numeric(16,4);
        DECLARE initprice numeric(16,4);
        DECLARE remainqty numeric(16,4);
        DECLARE remainprice numeric(16,4);
        DECLARE num_amount numeric(16,2);
        DECLARE cur_rows cursor for
          select b.ch_materialno, b.num_wrapping, b.num_present,  b.num_price, ifnull(c.int_unit_rate,1)
          from  cey_st_sheet_detail b, cey_bt_dish c
          where NEW.ch_sheetNo = b.ch_sheetno and b.ch_materialno = c.ch_dishno;
          DECLARE continue handler for not found set m_done = 1;
       open cur_rows;
       fetch cur_rows into  materialno, numnum, numpresent, numprice, unitrate;
     WHILE (m_done = 0) DO 
          IF(NEW.ch_state = 'Y')THEN
            set sheetNo = NEW.ch_sheetNo ;
            set sheetType = NEW.ch_sheetType ;
            set operation = NEW.ch_operation ;
            set dtaudit = NEW.dt_audit ;
            set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow  where ch_materialno = materialno);
            IF (intflowId = 0) THEN 
              set initqty =  0 ;
            ELSE
              set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);
            END IF;
            IF (intflowId = 0) THEN
              set initprice =  0 ;
            ELSE
              set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);
            END IF;
            IF (sheetType = 'RK')  THEN
                 set remainqty = initqty + numnum;
                 IF (remainqty = 0) THEN
                    set remainprice = 0;
                 ELSE
                    set remainprice = (abs(((numnum / unitrate) * numprice  + initqty * initprice) / remainqty));
                 END IF;
                 insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
                           num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)
                   values (materialno, now(), sheetType, 1, numnum, numprice / unitrate, remainqty, 0, remainqty, remainprice, sheetNo);
                 IF (numpresent > 0) THEN
                      set initqty = remainqty;
                      set initprice = remainprice;
                      set remainqty = initqty + numpresent;
                      IF (remainqty = 0) THEN
                         set remainprice = 0;
                      ELSEIF (initqty < 0 and remainqty > 0) THEN 
                             set remainprice = numprice / unitrate;
                      ELSE 
                            set remainprice = abs((initqty * initprice) / remainqty);
                      END IF;
                      insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
                             num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)
                      values (materialno, now(), sheetType, 1, numpresent, 0, remainprice, 0, remainqty, remainprice, sheetNo);
                 END IF;
                     IF exists ( Select * from cey_sys_parameter where vch_parameter = 'sys_stock_price' and vch_value = '1') THEN
                    update cey_bt_dish set num_sale_price = numprice where ch_dishno = materialno;
                 END IF;
                 IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN
                  update cey_st_material_num set num_num = remainqty, cost_price = remainprice
                   where ch_materialno = materialno;
                 ELSE 
                   insert into cey_st_material_num (ch_materialno, num_num, cost_price)
                     values (materialno, remainqty, remainprice);
                 END IF;
            ELSEIF (sheetType = 'TH')  THEN
                 set remainqty = initqty - numnum;
                 IF (remainqty = 0) THEN
                    set remainprice = 0;
                 ELSE
                    set remainprice = (abs((initqty * initprice - numnum / unitrate * numprice   ) / remainqty));
                 END IF;
                 insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
                           num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)
                   values (materialno, now(), sheetType, 1, numnum, numprice / unitrate, remainprice, 0, remainqty, remainprice, sheetNo);
                 IF (numpresent > 0) THEN 
                      set initqty   = remainqty;
                      set initprice = remainprice;
                      set remainqty = initqty - numpresent;
                      IF (remainqty = 0) THEN
                         set remainprice = 0;
                      ELSE
                           set remainprice = abs((initqty * initprice) / remainqty);
                      END IF;
                      insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
                             num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)
                      values (materialno, now(), sheetType, 1, numpresent, 0, remainprice, 0, remainqty, remainprice, sheetNo);
                  END IF;
                 IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN
                  update cey_st_material_num set num_num = remainqty, cost_price = remainprice
                   where ch_materialno = materialno;
                 ELSE 
                   insert into cey_st_material_num (ch_materialno, num_num, cost_price)
                     values (materialno, remainqty, remainprice);
                 END IF;
            ELSEIF (sheetType = 'CK') THEN
                 set remainqty = initqty - numnum;
                 IF (remainqty = 0) THEN
                    set remainprice = 0;
                 ELSEIF (initqty < 0 and remainqty > 0) THEN
                    set m_de = 1;
                 ELSE 
                    set remainprice = abs((initqty * initprice - numnum / unitrate * numprice   ) / remainqty);
                  END IF;
                 insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
                           num_num, sheet_price, cost_price, chg_amount, remain_qty, remain_price, ch_sheetno)
                   values (materialno, now(), sheetType, 1, numnum, numprice/unitrate, remainprice, 0, remainqty, remainprice, sheetNo);
                 IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN
                  update cey_st_material_num set num_num = remainqty, cost_price = remainprice
                   where ch_materialno = materialno;
                 ELSE 
                   insert into cey_st_material_num (ch_materialno, num_num, cost_price)
                     values (materialno, remainqty, remainprice);
                 END IF;
            END IF;
          END IF;
          fetch cur_rows into  materialno, numnum, numpresent, numprice, unitrate;
     END WHILE;
      close cur_rows;
    END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_sys_IPadress`
--

DROP TABLE IF EXISTS `cey_sys_IPadress`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_sys_IPadress` (
  `vch_ip` varchar(17) NOT NULL,
  `vch_hostname` varchar(100) NOT NULL,
  `dt_operdate` datetime NOT NULL,
  PRIMARY KEY (`vch_ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_sys_IPadress`
--

LOCK TABLES `cey_sys_IPadress` WRITE;
/*!40000 ALTER TABLE `cey_sys_IPadress` DISABLE KEYS */;
INSERT INTO `cey_sys_IPadress` VALUES ('','cz-desktop','2017-08-16 17:07:58'),('192.168.1.142','cz-desktop','2017-08-24 10:07:55');
/*!40000 ALTER TABLE `cey_sys_IPadress` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_sys_company`
--

DROP TABLE IF EXISTS `cey_sys_company`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_sys_company` (
  `vch_company` varchar(50) NOT NULL,
  `vch_principal` varchar(16) DEFAULT NULL,
  `vch_tel` varchar(16) DEFAULT NULL,
  `vch_handtel` varchar(16) DEFAULT NULL,
  `vch_email` varchar(40) DEFAULT NULL,
  `vch_address` varchar(50) DEFAULT NULL,
  `vch_tax_number` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`vch_company`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_sys_company`
--

LOCK TABLES `cey_sys_company` WRITE;
/*!40000 ALTER TABLE `cey_sys_company` DISABLE KEYS */;
INSERT INTO `cey_sys_company` VALUES ('********酒楼',NULL,NULL,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `cey_sys_company` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_sys_lan_terminal`
--

DROP TABLE IF EXISTS `cey_sys_lan_terminal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_sys_lan_terminal` (
  `vch_terminal` varchar(64) NOT NULL,
  `int_id` int(11) NOT NULL,
  `ch_billno` varchar(15) DEFAULT '',
  PRIMARY KEY (`vch_terminal`),
  UNIQUE KEY `int_id` (`int_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_sys_lan_terminal`
--

LOCK TABLES `cey_sys_lan_terminal` WRITE;
/*!40000 ALTER TABLE `cey_sys_lan_terminal` DISABLE KEYS */;
INSERT INTO `cey_sys_lan_terminal` VALUES ('AA81214063225521C',1,'');
/*!40000 ALTER TABLE `cey_sys_lan_terminal` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_sys_oper_purview`
--

DROP TABLE IF EXISTS `cey_sys_oper_purview`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_sys_oper_purview` (
  `vch_operID` varchar(6) NOT NULL,
  `ch_purviewno` varchar(4) NOT NULL,
  PRIMARY KEY (`ch_purviewno`,`vch_operID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_sys_oper_purview`
--

LOCK TABLES `cey_sys_oper_purview` WRITE;
/*!40000 ALTER TABLE `cey_sys_oper_purview` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_sys_oper_purview` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_sys_operator`
--

DROP TABLE IF EXISTS `cey_sys_operator`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_sys_operator` (
  `vch_operID` varchar(6) NOT NULL,
  `vch_operator` varchar(16) DEFAULT NULL,
  `vch_password` varchar(10) DEFAULT NULL,
  `ch_flag` varchar(1) DEFAULT 'N',
  `int_discount` int(11) DEFAULT '100',
  `ch_pre_mode` varchar(1) DEFAULT '1',
  `num_present` decimal(12,2) DEFAULT '0.00',
  `num_free` decimal(12,2) DEFAULT '0.00',
  `ch_waiter` varchar(1) DEFAULT 'N',
  `ch_free_flag` varchar(1) DEFAULT NULL,
  `num_shift_cash_register` decimal(12,2) DEFAULT '0.00',
  `ch_shift_config_flag` varchar(1) DEFAULT 'N',
  `num_deduct_rate` decimal(10,2) DEFAULT '0.00',
  PRIMARY KEY (`vch_operID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_sys_operator`
--

LOCK TABLES `cey_sys_operator` WRITE;
/*!40000 ALTER TABLE `cey_sys_operator` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_sys_operator` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_sys_parameter`
--

DROP TABLE IF EXISTS `cey_sys_parameter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_sys_parameter` (
  `vch_parameter` varchar(30) NOT NULL,
  `vch_value` text,
  `vch_explain` varchar(30) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT 'N',
  PRIMARY KEY (`vch_parameter`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_sys_parameter`
--

LOCK TABLES `cey_sys_parameter` WRITE;
/*!40000 ALTER TABLE `cey_sys_parameter` DISABLE KEYS */;
INSERT INTO `cey_sys_parameter` VALUES ('blotout_mode','1','抹零方式','N'),('blotout_round','1','抹零(1)或四舍五入(2)','N'),('bolt_operator','1','抹零默认当前操作员','N'),('bp_address','0','外卖地址的账单打印','N'),('bp_address_len','12','账单打印外卖地址宽度','N'),('bp_address_row','1','账单打印外卖地址行数','N'),('bp_amount','1','是否打印金额','N'),('bp_amount_dec','2','金额小位位','N'),('bp_amount_left','1','账单金额左空','N'),('bp_amount_len','9','金额列宽','N'),('bp_amount_row','2','金额行数','N'),('bp_avg','0','人均消费','N'),('bp_blotout','1','抹零金额是否打印','N'),('bp_cash','0','实收现金','N'),('bp_content_len','00','账单内容是否倍高倍宽','N'),('bp_cost','1','消费金额是否打印','N'),('bp_count','1','账单连续打印份数','N'),('bp_discount','1','折扣金额是否打印','N'),('bp_discountproject','0','会员折扣','N'),('bp_discount_left','1','账单折扣左空','N'),('bp_disdetail','0','是否打印折扣','N'),('bp_disdetail_len','6','折扣列宽','N'),('bp_disdetail_row','1','折扣行数','N'),('bp_dishname','1','是否打印品名','N'),('bp_dishname_left','1','账单品名左空','N'),('bp_dishname_len','18','品名列宽','N'),('bp_dishname_row','1','品名行数','N'),('bp_fmt_address','0',NULL,'N'),('bp_fmt_address_len','00',NULL,'N'),('bp_fmt_amount','1',NULL,'N'),('bp_fmt_amount_dec','2',NULL,'N'),('bp_fmt_amount_left','0',NULL,'N'),('bp_fmt_amount_len','6',NULL,'N'),('bp_fmt_amount_row','1',NULL,'N'),('bp_fmt_avg','0',NULL,'N'),('bp_fmt_avg_len','00',NULL,'N'),('bp_fmt_begincols','2',NULL,'N'),('bp_fmt_blotout','1',NULL,'N'),('bp_fmt_blotout_len','00',NULL,'N'),('bp_fmt_cash','1',NULL,'N'),('bp_fmt_cash_len','00',NULL,'N'),('bp_fmt_change','1',NULL,'N'),('bp_fmt_change_len','00',NULL,'N'),('bp_fmt_content_len','00',NULL,'N'),('bp_fmt_cost','1',NULL,'N'),('bp_fmt_cost_len','00',NULL,'N'),('bp_fmt_discount','1',NULL,'N'),('bp_fmt_discount_left','0',NULL,'N'),('bp_fmt_discount_len','00',NULL,'N'),('bp_fmt_disdetail','0',NULL,'N'),('bp_fmt_disdetail_len','6',NULL,'N'),('bp_fmt_disdetail_row','1',NULL,'N'),('bp_fmt_dishname','1',NULL,'N'),('bp_fmt_dishname_left','0',NULL,'N'),('bp_fmt_dishname_len','20',NULL,'N'),('bp_fmt_dishname_row','1',NULL,'N'),('bp_fmt_end_bc','1245',NULL,'N'),('bp_fmt_end_bc_en','0',NULL,'N'),('bp_fmt_end_desc','欢迎光临',NULL,'N'),('bp_fmt_end_rc','1245',NULL,'N'),('bp_fmt_end_rc_en','1',NULL,'N'),('bp_fmt_free','1',NULL,'N'),('bp_fmt_free_len','00',NULL,'N'),('bp_fmt_linkman','0',NULL,'N'),('bp_fmt_linkman_len','00',NULL,'N'),('bp_fmt_lowcost','1',NULL,'N'),('bp_fmt_lowcost_len','00',NULL,'N'),('bp_fmt_memo','0',NULL,'N'),('bp_fmt_memo_len','00',NULL,'N'),('bp_fmt_m_adress','1',NULL,'N'),('bp_fmt_m_amount','1',NULL,'N'),('bp_fmt_m_detail','1',NULL,'N'),('bp_fmt_m_detail_len','00',NULL,'N'),('bp_fmt_m_name','1',NULL,'N'),('bp_fmt_m_no','1',NULL,'N'),('bp_fmt_m_point','1',NULL,'N'),('bp_fmt_num','1',NULL,'N'),('bp_fmt_num_dec','3',NULL,'N'),('bp_fmt_num_left','0',NULL,'N'),('bp_fmt_num_len','6',NULL,'N'),('bp_fmt_num_row','1',NULL,'N'),('bp_fmt_offcols','2',NULL,'N'),('bp_fmt_operid','1',NULL,'N'),('bp_fmt_operid_len','00',NULL,'N'),('bp_fmt_overstep_cut','0',NULL,'N'),('bp_fmt_payno','1',NULL,'N'),('bp_fmt_payno_len','00',NULL,'N'),('bp_fmt_pay_detail','1',NULL,'N'),('bp_fmt_pay_detail_len','00',NULL,'N'),('bp_fmt_person','0',NULL,'N'),('bp_fmt_person_len','00',NULL,'N'),('bp_fmt_present','1',NULL,'N'),('bp_fmt_presentflag','1',NULL,'N'),('bp_fmt_present_len','00',NULL,'N'),('bp_fmt_price','0',NULL,'N'),('bp_fmt_price_dec','2',NULL,'N'),('bp_fmt_price_left','0',NULL,'N'),('bp_fmt_price_len','7',NULL,'N'),('bp_fmt_price_row','1',NULL,'N'),('bp_fmt_printmemo','1',NULL,'N'),('bp_fmt_receive','1',NULL,'N'),('bp_fmt_receive_len','00',NULL,'N'),('bp_fmt_rowid','0',NULL,'N'),('bp_fmt_serial_no','1',NULL,'N'),('bp_fmt_serial_no_len','00',NULL,'N'),('bp_fmt_series_group','2',NULL,'N'),('bp_fmt_service','1',NULL,'N'),('bp_fmt_service_len','00',NULL,'N'),('bp_fmt_specialflag','1',NULL,'N'),('bp_fmt_table','1',NULL,'N'),('bp_fmt_table_len','00',NULL,'N'),('bp_fmt_tail_block','0',NULL,'N'),('bp_fmt_tax','1',NULL,'N'),('bp_fmt_tax_len','00',NULL,'N'),('bp_fmt_tel','0',NULL,'N'),('bp_fmt_tel_len','00',NULL,'N'),('bp_fmt_time','1',NULL,'N'),('bp_fmt_times','0',NULL,'N'),('bp_fmt_times_len','00',NULL,'N'),('bp_fmt_time_len','00',NULL,'N'),('bp_fmt_title_disable','1',NULL,'N'),('bp_fmt_title_logo','0',NULL,'N'),('bp_fmt_title_logo_en','0',NULL,'N'),('bp_fmt_title_sc_enable','1',NULL,'N'),('bp_fmt_title_second','必胜客',NULL,'N'),('bp_fmt_top_desc','付款单\n新开小店，欢迎品尝\n商家地址：\n南京市栖霞区文苑路与学思路路口文苑广场\n',NULL,'N'),('bp_fmt_underwrite','1',NULL,'N'),('bp_fmt_unit','0',NULL,'N'),('bp_fmt_unit_left','0',NULL,'N'),('bp_fmt_unit_len','13',NULL,'N'),('bp_fmt_unit_row','1',NULL,'N'),('bp_fmt_waiter','0',NULL,'N'),('bp_fmt_waitername','0',NULL,'N'),('bp_fmt_waitername_len','00',NULL,'N'),('bp_fmt_waiter_len','00',NULL,'N'),('bp_free','0','免单金额是否打印','N'),('bp_identifier','1','是否打印账单标识','N'),('bp_linkman','0','外卖联系电话的账单打印','N'),('bp_linkman_len','12','账单打印外卖联系电话宽度','N'),('bp_linkman_row','1','账单打印外卖联系电话行数','N'),('bp_lowcost','0','低消金额是否打印','N'),('bp_membername','0','会员名称','N'),('bp_membertype','0','会员类型','N'),('bp_memo','0','开台备注','N'),('bp_memo_len','12','开台备注打印宽度','N'),('bp_memo_row','1','开台备注打印行数','N'),('bp_num','1','是否打印数量','N'),('bp_num_dec','2','数量小位位','N'),('bp_num_left','1','账单数量左空','N'),('bp_num_len','9','数量列宽','N'),('bp_num_row','2','数量行数','N'),('bp_offcols','2','表尾每行打印信息数','N'),('bp_operid','1','工号的账单打印','N'),('bp_operid_len','12','账单打印工号宽度','N'),('bp_operid_row','2','账单打印工号行数','N'),('bp_overstep_cut','1','超出字符设置截取','N'),('bp_payno','1','付款单号的账单打印','N'),('bp_payno_len','16','账单打印单号宽度','N'),('bp_payno_row','1','账单打印单号行数','N'),('bp_person','0','人数的账单打印','N'),('bp_person_len','12','账单打印人数宽度','N'),('bp_person_row','1','账单打印人数行数','N'),('bp_present','1','赠送金额是否打印','N'),('bp_presentflag','0','是否打印赠送标识','N'),('bp_price','1','是否打印价格','N'),('bp_price_dec','2','价格小位位','N'),('bp_price_left','1','账单价格左空','N'),('bp_price_len','11','价格列宽','N'),('bp_price_row','2','价格行数','N'),('bp_printmemo','0','是否厨打说明','N'),('bp_print_mode','1','账单内容打印方式','N'),('bp_receive','1','应收金额是否打印','N'),('bp_rowid','0','是否打印行序号','N'),('bp_series_group','0','按大类分组','N'),('bp_service','0','服务费金额是否打印','N'),('bp_specialflag','0','是否打印特价标识','N'),('bp_table','1','餐桌的账单打印','N'),('bp_table_len','12','账单打印餐桌宽度','N'),('bp_table_row','2','账单打印餐桌行数','N'),('bp_tel','0','外卖联系电话的账单打印','N'),('bp_tel_len','12','账单打印外卖联系电话宽度','N'),('bp_tel_row','1','账单打印外卖联系电话行数','N'),('bp_time','0','结账时间的账单打印','N'),('bp_times','0','时长的账单打印','N'),('bp_times_len','12','账单打印时长宽度','N'),('bp_times_row','1','账单打印时长行数','N'),('bp_time_len','12','账单打印结账时间宽度','N'),('bp_time_row','1','账单打印结账时间行数','N'),('bp_togo_count','1','外卖帐单连续打印份数','N'),('bp_underwrite','1','签名是否打印','N'),('bp_unit','0','是否打印单位','N'),('bp_unit_left','1','账单品码左空','N'),('bp_unit_len','13','单位列宽','N'),('bp_unit_row','2','单位行数','N'),('bp_waiter','0','结算服务员打印','N'),('bp_waitername','0','服务员名称打印','N'),('bp_waitername_len','12','服务员名称打印宽度','N'),('bp_waitername_row','1','服务员名称打印行数','N'),('bp_waiter_len','12','结算服务员打印宽度','N'),('bp_waiter_row','1','结算服务员打印行数','N'),('cash_payall','1','现金默认付全款','N'),('cp_back','1','厨打退菜标识','N'),('cp_billno','0','厨打开台单号','N'),('cp_color','0','退菜记录厨打为红色','N'),('cp_detailid','1','厨打本单序号','N'),('cp_detail_fsize','11','厨打明细字体大小','N'),('cp_dishname','1','厨打菜品名称','N'),('cp_dishname_row','1','厨打菜品名称的行数','N'),('cp_dishname_size','20','厨打菜品名称字符数','N'),('cp_dishno','0','厨打菜品编码','N'),('cp_dishno_row','2','厨打菜品编码行数','N'),('cp_dishno_size','8','厨打菜品编码符数','N'),('cp_id','1','厨打序号','N'),('cp_num','1','厨打菜品数量','N'),('cp_num_dec','2','厨打数量小数位','N'),('cp_num_row','1','厨打菜品数量的行数','N'),('cp_num_size','4','厨打菜品数量字符数','N'),('cp_offamount','0','表尾厨打金额合计','N'),('cp_operator','0','厨打点菜人','N'),('cp_ordertime','1','厨打点菜时间','N'),('cp_person','0','打印人数','N'),('cp_present','1','厨打奉送标识','N'),('cp_price','0','厨打菜品价格','N'),('cp_price_dec','2','厨打价格小数位','N'),('cp_price_row','1','厨打菜品价格的行数','N'),('cp_price_size','7','厨打菜品价格字符数','N'),('cp_printtime','0','厨打时间','N'),('cp_suit_print','3','套菜打印方式','N'),('cp_table','1','厨打餐桌名称','N'),('cp_title','0','厨打方案名称','N'),('cp_title1_fsize','11','厨打表头字体大小','N'),('cp_title_1','1','点菜单类型名称','N'),('cp_title_fsize','14','厨打标题字体大小','N'),('cp_trans_print','0','转台厨打','N'),('cp_unit','1','厨打单位','N'),('cp_unit_row','1','厨打单位的行数','N'),('cp_unit_size','6','厨打单位字符数','N'),('cp_width','40','厨打纸张宽度','N'),('DataBase_Version','V1_140101','系统数据库版本','N'),('handover_oper','0','交班必需接班人','N'),('kt_checkBox_ch_billno','1',NULL,'N'),('kt_checkBox_ch_checkprintstate','1',NULL,'N'),('kt_checkBox_ch_rowno','1',NULL,'N'),('kt_checkBox_ch_serialno','1',NULL,'N'),('kt_checkBox_ch_tableno','1',NULL,'N'),('kt_checkBox_ch_tailtotal','0',NULL,'N'),('kt_checkBox_ch_tailtotal_scale','11',NULL,'N'),('kt_checkBox_dt_cur','1',NULL,'N'),('kt_checkBox_dt_operdate','1',NULL,'N'),('kt_checkBox_int_person','1',NULL,'N'),('kt_checkBox_num_num','1',NULL,'N'),('kt_checkBox_num_price','0',NULL,'N'),('kt_checkBox_title','0',NULL,'N'),('kt_checkBox_vch_dishname','1',NULL,'N'),('kt_checkBox_vch_operid','1',NULL,'N'),('kt_checkBox_vch_plan_id','0',NULL,'N'),('kt_lineEdit_ch_billno_scale','00',NULL,'N'),('kt_lineEdit_ch_serialno_scale','11',NULL,'N'),('kt_lineEdit_ch_tableno_scale','11',NULL,'N'),('kt_lineEdit_content_scale','01',NULL,'N'),('kt_lineEdit_dt_cur_scale','00',NULL,'N'),('kt_lineEdit_dt_operdate_scale','00',NULL,'N'),('kt_lineEdit_int_person_scale','00',NULL,'N'),('kt_lineEdit_vch_operid_scale','00',NULL,'N'),('kt_spinBox_num_num_row','1',NULL,'N'),('kt_spinBox_num_num_width','15',NULL,'N'),('kt_spinBox_num_price_row','1',NULL,'N'),('kt_spinBox_num_price_width','15',NULL,'N'),('kt_spinBox_unit_row','1',NULL,'N'),('kt_spinBox_unit_width','15',NULL,'N'),('kt_spinBox_vch_dishname_row','1',NULL,'N'),('kt_spinBox_vch_dishname_width','15',NULL,'N'),('kt_spinBox_vtimeout','1',NULL,'N'),('label_address','0','标签打印外卖地址','N'),('label_address_line','','标签外卖地址打印所在行','N'),('label_billno','0','标签打印单号','N'),('label_billno_line','','标签单号打印所在行','N'),('label_dishname','0','标签打印品名','N'),('label_dishname_line','','标签品名打印所在行','N'),('label_memo','0','标签打印做法','N'),('label_memo_line','','标签做法打印所在行','N'),('label_model','','标签打印模板','N'),('label_price','0','标签打印价格','N'),('label_price_line','','标签价格打印所在行','N'),('label_tel','0','标签打印外卖电话','N'),('label_tel_line','','标签外卖电话打印所在行','N'),('label_time','0','标签打印时间','N'),('label_time_line','','标签时间打印所在行','N'),('label_title','0','标签打印抬头','N'),('label_title1','0','标签打印标题','N'),('label_title1_line','','标签标题打印所在行','N'),('label_title_line','','标签抬头打印所在行','N'),('last_wirelessno','0','无线点菜最后流水','N'),('low_service','0','服务费记入低消差','N'),('negative','0','点菜数量不充许为负数','N'),('orderdish_back','0','退菜必须输入退菜原因','N'),('orderdish_hf','0','编码点菜启用半模糊查询','N'),('ordertonum','0','点菜后自动进入数量录入','N'),('pay_auto_discount','0','结账自动折扣','N'),('pay_auto_discount_value','100','结账自动折扣值','N'),('print_fashion','2','帐单打印发送方式','N'),('print_vip_point','0','会员打印积分','N'),('print_ye','0','会员打印余额','N'),('public_short_cut_map_ADD','E,69',NULL,'N'),('public_short_cut_map_CYC','N,78',NULL,'N'),('public_short_cut_map_CZ','R,82',NULL,'N'),('public_short_cut_map_DJQ','J,74',NULL,'N'),('public_short_cut_map_DZDP','D,68',NULL,'N'),('public_short_cut_map_GD','I,73',NULL,'N'),('public_short_cut_map_HC','H,72',NULL,'N'),('public_short_cut_map_HY','M,77',NULL,'N'),('public_short_cut_map_HYCZ','R,82',NULL,'N'),('public_short_cut_map_HYGZ','G,71',NULL,'N'),('public_short_cut_map_HYK','M,77',NULL,'N'),('public_short_cut_map_JB','J,74',NULL,'N'),('public_short_cut_map_JS','P,80',NULL,'N'),('public_short_cut_map_KQX','S,83',NULL,'N'),('public_short_cut_map_MT','V,86',NULL,'N'),('public_short_cut_map_QCZK','Z,90',NULL,'N'),('public_short_cut_map_QHHT','B,66',NULL,'N'),('public_short_cut_map_QZ','W,87',NULL,'N'),('public_short_cut_map_SC','D,68',NULL,'N'),('public_short_cut_map_SP','L,76',NULL,'N'),('public_short_cut_map_SUB','Q,81',NULL,'N'),('public_short_cut_map_TCTK','K,75',NULL,'N'),('public_short_cut_map_WX','W,87',NULL,'N'),('public_short_cut_map_XG','C,67',NULL,'N'),('public_short_cut_map_XGMM','O,79',NULL,'N'),('public_short_cut_map_YH','H,72',NULL,'N'),('public_short_cut_map_YHK','Y,89',NULL,'N'),('public_short_cut_map_ZDQX','X,88',NULL,'N'),('public_short_cut_map_ZFB','Q,81',NULL,'N'),('public_short_cut_map_ZK','Z,90',NULL,'N'),('public_short_cut_map_ZS','G,71',NULL,'N'),('round_mode','2','四舍五入方式','N'),('serial_no','1','是否进行流水号打印','N'),('serial_no_begin','0','开始流水号','N'),('serial_no_end','999','结束流水号','N'),('serial_no_len','12','流水号宽度','N'),('serial_no_now','0','开始流水号','N'),('serial_no_row','1','流水号列数','N'),('service_handwork','0','手工处理服务费','N'),('shortorder_tableno','0','快餐必须输入台(牌)号','N'),('store_print_total','table','库存单据打印合计金额位置','N'),('system_pass','0000','系统管理员密码','N'),('sys_business_type','2','经营业态','N'),('sys_stock_price','0','用入库价更新采购价','N'),('togo_pay_atonce','0','外卖落单立即结账','N');
/*!40000 ALTER TABLE `cey_sys_parameter` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_cey_sys_parameter_update BEFORE UPDATE ON cey_sys_parameter FOR EACH ROW  Begin   IF(NEW.vch_parameter = OLD.vch_parameter and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_sys_purview_list`
--

DROP TABLE IF EXISTS `cey_sys_purview_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_sys_purview_list` (
  `ch_purviewno` varchar(4) NOT NULL,
  `vch_purviewname` varchar(80) NOT NULL,
  `ch_day_cue` varchar(1) DEFAULT NULL,
  `ch_open_type` varchar(4) DEFAULT NULL,
  `vch_window` varchar(40) DEFAULT NULL,
  `vch_purview_typename` varchar(80) DEFAULT NULL,
  `ch_purview_typeno` varchar(2) DEFAULT NULL,
  `ch_report` varchar(1) DEFAULT 'N',
  `vch_withparm` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`ch_purviewno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_sys_purview_list`
--

LOCK TABLES `cey_sys_purview_list` WRITE;
/*!40000 ALTER TABLE `cey_sys_purview_list` DISABLE KEYS */;
INSERT INTO `cey_sys_purview_list` VALUES ('0101','菜品资料维护                  ','N','SHEE','w_bt_dish','数据维护','01','N','NULL'),('0102','套菜资料维护                  ','N','SHEE','w_bt_dish_suit','数据维护','01','N','NULL'),('0103','菜品厨打设置                  ','N','SHEE','w_bt_dish_cp','数据维护','01','N','NULL'),('0104','菜品做法设置                  ','N','SHEE','w_bt_print_memo','数据维护','01','N','NULL'),('0105','餐桌资料维护                  ','N','SHEE','w_bt_table','数据维护','01','N','NULL'),('0106','收银方式维护                  ','N','EDIT','w_bt_paymode','数据维护','01','N','NULL'),('0107','营业班次                      ','N','EDIT','w_bt_class','数据维护','01','N','NULL'),('0110','特殊操作原因                  ','N','SHEE','w_bt_other','数据维护','01','N','NULL'),('0201','营业单据查询                  ','N','EDIT','w_u_bill_search','营业管理','02','N','NULL'),('0202','当前台况统计                  ','N','EDIT','w_u_curtable','营业管理','02','N','NULL'),('0203','菜品销售流水查询              ','N','SHEE','w_rpt_cost_flow','营业管理','02','N','NULL'),('0204','收银流水查询                  ','N','SHEE','w_rpt_checkout_flow','营业管理','02','N','NULL'),('0205','菜品历史特价查询              ','N','SHEE','w_rpt_dish_special','营业管理','02','N','NULL'),('0206','菜品历史沽清查询              ','N','SHEE','w_rpt_dish_warn','营业管理','02','N','NULL'),('0207','外卖收银结算                  ','N','EDIT','w_u_togo_balance','营业管理','02','N','NULL'),('0208','餐台锁态管理                  ','N','EDIT','w_u_table_lock','营业管理','02','N','NULL'),('0209','开钱箱                        ','O','EDIT','NONE','营业管理','02','N','NULL'),('0210','开台                          ','O','EDIT','NONE','营业管理','02','N','NULL'),('0211','转台                          ','O','EDIT','NONE','营业管理','02','N','NULL'),('0212','消台                          ','O','EDIT','NONE','营业管理','02','N','NULL'),('0213','点菜                          ','O','EDIT','NONE','营业管理','02','N','NULL'),('0214','结算                          ','O','EDIT','NONE','营业管理','02','N','NULL'),('0215','厨房催菜                      ','O','EDIT','NONE','营业管理','02','N','NULL'),('0216','菜品转台                      ','O','EDIT','NONE','营业管理','02','N','NULL'),('0217','接班操作                      ','O','EDIT','NONE','营业管理','02','N','NULL'),('0218','退菜                          ','O','EDIT','NONE','营业管理','02','N','NULL'),('0219','免服务费                      ','O','EDIT','NONE','营业管理','02','N','NULL'),('0220','免低消差                      ','O','EDIT','NONE','营业管理','02','N','NULL'),('0221','预打账单                      ','O','EDIT','NONE','营业管理','02','N','NULL'),('0222','快餐挂单                      ','O','EDIT','NONE','营业管理','02','N','NULL'),('0223','快餐外卖                      ','O','EDIT','NONE','营业管理','02','N','NULL'),('0224','前台营业情况统计              ','O','EDIT','NONE','营业管理','02','N','NULL'),('0225','外卖退货                      ','O','EDIT','NONE','营业管理','02','N','NULL'),('0226','增加临时菜                    ','O','EDIT','NONE','营业管理','02','N','NULL'),('0227','反结账                        ','O','EDIT','NONE','营业管理','02','N','NULL'),('0228','退单                          ','O','EDIT','NONE','营业管理','02','N','NULL'),('0240','营业日结                      ','N','EDIT','w_ub_day_sumup','营业管理','02','N','NULL'),('0241','前台营业                      ','N','SHEE','w_mdi_qt','营业管理','02','N','NULL'),('0242','今日特价维护                  ','N','SHEE','w_bt_dish_special','营业管理','02','N','NULL'),('0243','今日菜品沽清                  ','N','SHEE','w_bt_dish_warn','营业管理','02','N','NULL'),('0244','	后台                  	','N','EDIT','w_sys_parameter	','营业管理	','02','N','	NULL'),('0401','会员参数设置                  ','N','EDIT','w_m_parameter','会员管理','04','N','NULL'),('0402','会员资料维护                  ','N','SHEE','w_m_member_list','会员管理','04','N','NULL'),('0403','会员状态管理                  ','N','SHEE','w_m_manage','会员管理','04','N','NULL'),('0404','充值会员充值                  ','N','EDIT','w_m_member_deposit','会员管理','04','N','NULL'),('0406','会员积分管理                  ','N','SHEE','w_m_point_manage','会员管理','04','N','NULL'),('0407','会员类型变更                  ','N','EDIT','w_m_type_change','会员管理','04','N','NULL'),('0411','会员报表                      ','N','SHEE','w_rpt_member_total','会员管理','04','N','NULL'),('0413','会员卡芯片清除                ','N','EDIT','w_m_icblankout','会员管理','04','N','NULL'),('0602','商品入库                      ','N','SHEE','w_st_voucher_list','库存管理','06','N','RK'),('0603','商品退货单                    ','N','SHEE','w_st_voucher_list','库存管理','06','N','TH'),('0604','报损单                        ','N','SHEE','w_st_voucher_list','库存管理','06','N','CK'),('0605','库存盘点                      ','N','SHEE','w_st_store_check','库存管理','06','N','NULL'),('0606','库存查询                      ','N','SHEE','w_st_rpt_store_num','库存管理','06','N','NULL'),('0609','盘点查询                      ','N','SHEE','w_st_rpt_material_check_query','库存管理','06','N','RK'),('0610','进销存汇总报表                ','N','SHEE','w_st_material_jxc','库存管理','06','N','NULL'),('0611','单据汇总查询                  ','N','SHEE','w_st_rpt_total','库存管理','06','N','NULL'),('0612','库存参数设置                  ','N','EDIT','w_st_parameter','库存管理','06','N','NULL'),('0701','菜品消费报表                  ','N','SHEE','w_rpt_cost','报表中心','07','N','NULL'),('0702','菜品促销报表                  ','N','SHEE','w_rpt_cost_favourable','报表中心','07','N','NULL'),('0703','餐桌营业数据统计              ','N','SHEE','w_rpt_cost_tableno','报表中心','07','N','NULL'),('0704','退菜情况报表                  ','N','SHEE','w_rpt_cost_back','报表中心','07','N','NULL'),('0705','营业收银报表                  ','N','SHEE','w_rpt_checkout','报表中心','07','N','NULL'),('0706','服务费与低消情况查询          ','N','SHEE','w_rpt_service_low','报表中心','07','N','NULL'),('0707','收银免单情况查询              ','N','SHEE','w_rpt_checkout_free','报表中心','07','N','NULL'),('0708','日月周报表分析                ','N','SHEE','w_rpt_dwm','报表中心','07','N','NULL'),('0710','日营业情况统计                ','N','SHEE','w_rpt_day','报表中心','07','N','NULL'),('0711','营业日销售情况统计            ','N','SHEE','w_rpt_day_series','报表中心','07','N','NULL'),('0712','品类销售营业分析              ','N','SHEE','w_rpt_cost_series','报表中心','07','N','NULL'),('0713','时段营业分析                  ','N','SHEE','w_rpt_period','报表中心','07','N','NULL'),('0714','菜品销售排行统计              ','N','SHEE','w_rpt_cost_order','报表中心','07','N','NULL'),('0715','服务员提成查询                ','N','SHEE','w_rpt_cost_waiter','报表中心','07','N','NULL'),('0719','反结帐查询                    ','N','SHEE','w_rpt_uncheckout_flow','报表中心','07','N','NULL'),('0720','直销商品毛利情况表            ','N','SHEE','w_rpt_dish_gross_profit','报表中心','07','N','NULL'),('0901','公司信息设置                  ','N','EDIT','w_sys_company','系统管理','09','N','NULL'),('0902','系统参数设置                  ','N','EDIT','w_sys_parameter','营业管理','02','N','NULL'),('0903','操作员资料维护                ','N','EDIT','w_sys_operator','系统管理','09','N','NULL'),('0904','操作权限维护                  ','N','EDIT','w_sys_purview','系统管理','09','N','NULL'),('0905','数据库备份                    ','N','EDIT','w_sys_backup','系统管理','09','N','NULL'),('0906','基础数据导出                  ','N','EDIT','w_sys_data_export','系统管理','09','N','NULL'),('0907','基础数据导入                  ','N','EDIT','w_sys_data_import','系统管理','09','N','NULL'),('0908','业务数据清除                  ','N','EDIT','w_sys_data_clear','系统管理','09','N','NULL'),('0909','权限设置                      ','N','SHEE','w_sys_purview','系统管理','09','N','NULL'),('0910','MySQL服务器','N','EDIT',NULL,'系统管理','09','N',NULL),('0911','文件管理器','N','EDIT',NULL,'系统管理','09','N',NULL),('0912','无线点菜监控','N','EDIT',NULL,'系统管理','09','N',NULL),('0913','云同步','N','EDIT',NULL,'系统管理','09','N',NULL),('1501','短信组管理                    ','N','EDIT','w_ms_group_form','短信管理','15','N','NULL'),('1502','短信成员管理                  ','N','EDIT','w_ms_cust_input','短信管理','15','N','NULL'),('1503','短信设置                      ','N','EDIT','w_ms_message_setup','短信管理','15','N','NULL'),('1504','短信单发                      ','N','EDIT','w_ms_send_single','短信管理','15','N','NULL'),('1505','短信群发                      ','N','EDIT','w_ms_send_multi','短信管理','15','N','NULL'),('1506','短信查询                      ','N','EDIT','w_ms_query_send','短信管理','15','N','NULL');
/*!40000 ALTER TABLE `cey_sys_purview_list` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_sys_sqlversion`
--

DROP TABLE IF EXISTS `cey_sys_sqlversion`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_sys_sqlversion` (
  `sqlversion` varchar(255) NOT NULL,
  `sqlupdatetime` datetime DEFAULT NULL,
  `sqlversion_sub` int(5) DEFAULT '0',
  PRIMARY KEY (`sqlversion`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_sys_sqlversion`
--

LOCK TABLES `cey_sys_sqlversion` WRITE;
/*!40000 ALTER TABLE `cey_sys_sqlversion` DISABLE KEYS */;
INSERT INTO `cey_sys_sqlversion` VALUES ('1.1','2019-11-13 11:44:43',0),('1.2','2019-11-13 11:44:43',0),('1.3','2019-11-13 11:44:43',0),('1.4','2019-11-13 11:44:43',0),('1.5','2019-11-13 11:44:43',0),('1.6','2019-11-13 11:44:43',0),('1.7','2019-11-13 11:44:43',0),('1.8','2019-11-13 11:44:43',0),('1.9','2019-11-13 11:44:43',0),('2.0','2019-11-13 11:44:43',0),('2.1','2019-11-13 11:44:43',0),('2.2','2019-11-13 11:44:43',0),('2.3','2019-11-13 11:44:44',0),('2.4','2019-11-13 11:44:44',0),('2.5','2019-11-13 11:44:44',0),('2.6','2019-11-13 11:44:44',0),('2.7','2019-11-13 11:44:44',0),('2.8','2019-11-13 11:44:44',0),('2.9','2019-11-13 11:44:44',0),('3.0','2019-11-13 11:44:44',0),('3.1','2019-11-13 11:44:47',0),('3.2','2019-11-13 11:44:47',0),('3.3','2019-11-13 11:44:48',0),('3.4','2019-11-13 11:44:48',0),('3.5','2019-11-13 11:44:48',0),('3.6','2019-11-13 11:44:48',0),('3.7','2019-11-13 11:44:48',0),('3.8','2019-11-13 11:44:48',0),('3.9','2019-11-13 11:44:48',0),('4.0','2019-11-13 11:44:48',0),('4.1','2019-11-13 11:44:48',19),('4.2','2019-11-13 11:44:48',0),('4.3','2019-11-13 11:44:49',0),('4.4','2019-11-13 11:44:49',0),('4.5','2019-11-13 11:44:49',0),('4.6','2019-11-13 11:44:49',0),('4.7','2019-11-13 11:44:49',0),('4.8','2019-11-13 11:44:49',0),('4.9','2019-11-13 11:44:49',0),('5.0','2019-11-13 11:44:49',0),('5.1','2019-11-13 11:44:49',30);
/*!40000 ALTER TABLE `cey_sys_sqlversion` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_checkout_detail`
--

DROP TABLE IF EXISTS `cey_u_checkout_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_checkout_detail` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `ch_payno` varchar(15) NOT NULL,
  `ch_paymodeno` varchar(2) NOT NULL,
  `num_payamount` decimal(12,2) DEFAULT NULL,
  `num_change` decimal(12,2) DEFAULT '0.00',
  `num_realamount` decimal(12,2) DEFAULT '0.00',
  `num_face` decimal(12,2) DEFAULT '0.00',
  `vch_voucherno` varchar(30) DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `num_card_remain` decimal(12,2) DEFAULT '0.00',
  `num_face_Num` decimal(12,2) NOT NULL DEFAULT '0.00',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `terminal_code` varchar(128) DEFAULT '',
  `vch_card_name` varchar(40) DEFAULT NULL,
  `num_card_point` decimal(12,2) DEFAULT NULL,
  `vch_card_adress` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`),
  KEY `idx_cey_u_checkout_detail_1` (`ch_payno`),
  KEY `idx_cey_u_checkout_detail_2` (`dt_operdate`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_checkout_detail`
--

LOCK TABLES `cey_u_checkout_detail` WRITE;
/*!40000 ALTER TABLE `cey_u_checkout_detail` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_checkout_detail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_checkout_master`
--

DROP TABLE IF EXISTS `cey_u_checkout_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_checkout_master` (
  `ch_payno` varchar(15) NOT NULL,
  `ch_billno` varchar(15) NOT NULL,
  `ch_tableno` varchar(4) NOT NULL,
  `num_cost` decimal(12,2) DEFAULT NULL,
  `num_discount` decimal(12,2) DEFAULT NULL,
  `num_present` decimal(12,2) DEFAULT NULL,
  `num_service` decimal(12,2) DEFAULT NULL,
  `num_lowcost` decimal(12,2) DEFAULT NULL,
  `num_blotout` decimal(12,2) DEFAULT NULL,
  `num_free` decimal(12,2) DEFAULT NULL,
  `vch_free_operID` varchar(6) DEFAULT NULL,
  `num_cash` decimal(12,2) DEFAULT NULL,
  `num_other` decimal(12,2) DEFAULT NULL,
  `num_run` decimal(12,2) DEFAULT NULL,
  `vch_run_operID` varchar(6) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_operID2` varchar(6) DEFAULT NULL,
  `dt_operdate2` datetime DEFAULT NULL,
  `vch_memberno` varchar(20) DEFAULT NULL,
  `ch_type` varchar(1) DEFAULT 'N',
  `num_rate` decimal(12,2) DEFAULT NULL,
  `vch_service_desc` varchar(32) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `ch_member_belong_state` varchar(1) DEFAULT NULL,
  `ch_yun_payno` varchar(15) DEFAULT NULL,
  `terminal_code` varchar(128) DEFAULT '',
  PRIMARY KEY (`ch_payno`),
  KEY `idx_cey_u_checkout_master_1` (`ch_billno`),
  KEY `idx_cey_u_checkout_master_2` (`ch_tableno`),
  KEY `idx_cey_u_checkout_master_3` (`dt_operdate`),
  KEY `idx_cey_u_checkout_master_4` (`ch_state`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_checkout_master`
--

LOCK TABLES `cey_u_checkout_master` WRITE;
/*!40000 ALTER TABLE `cey_u_checkout_master` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_checkout_master` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_day_sumup`
--

DROP TABLE IF EXISTS `cey_u_day_sumup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_day_sumup` (
  `dt_date` datetime NOT NULL,
  `dt_begin` datetime NOT NULL,
  `dt_end` datetime NOT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_upflag` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`dt_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_day_sumup`
--

LOCK TABLES `cey_u_day_sumup` WRITE;
/*!40000 ALTER TABLE `cey_u_day_sumup` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_day_sumup` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_handover_detail`
--

DROP TABLE IF EXISTS `cey_u_handover_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_handover_detail` (
  `int_id` int(11) NOT NULL,
  `vch_paymode` varchar(60) NOT NULL,
  `int_count` int(11) DEFAULT NULL,
  `num_amount` decimal(12,2) DEFAULT NULL,
  `num_face` decimal(12,2) DEFAULT NULL,
  PRIMARY KEY (`int_id`,`vch_paymode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_handover_detail`
--

LOCK TABLES `cey_u_handover_detail` WRITE;
/*!40000 ALTER TABLE `cey_u_handover_detail` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_handover_detail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_handover_master`
--

DROP TABLE IF EXISTS `cey_u_handover_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_handover_master` (
  `int_id` int(11) NOT NULL,
  `vch_operID` varchar(20) NOT NULL,
  `dt_begin` datetime NOT NULL,
  `dt_end` datetime NOT NULL,
  `vch_accept` varchar(60) DEFAULT NULL,
  `vch_memo` varchar(100) DEFAULT NULL,
  `vch_memo2` varchar(1023) DEFAULT '',
  PRIMARY KEY (`int_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_handover_master`
--

LOCK TABLES `cey_u_handover_master` WRITE;
/*!40000 ALTER TABLE `cey_u_handover_master` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_handover_master` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_handover_total`
--

DROP TABLE IF EXISTS `cey_u_handover_total`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_handover_total` (
  `int_id` int(11) NOT NULL,
  `int_row` int(11) NOT NULL,
  `vch_item` varchar(100) NOT NULL,
  `num_amount` decimal(12,2) DEFAULT NULL,
  `vch_item2` varchar(100) DEFAULT NULL,
  `num_amount2` decimal(12,2) DEFAULT NULL,
  `int_height` int(11) DEFAULT '84',
  PRIMARY KEY (`int_id`,`int_row`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_handover_total`
--

LOCK TABLES `cey_u_handover_total` WRITE;
/*!40000 ALTER TABLE `cey_u_handover_total` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_handover_total` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_hungbill`
--

DROP TABLE IF EXISTS `cey_u_hungbill`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_hungbill` (
  `int_flowID` int(11) NOT NULL,
  `int_id` int(11) NOT NULL,
  `ch_tableno` varchar(4) DEFAULT NULL,
  `ch_dishno` varchar(7) DEFAULT NULL,
  `num_price` decimal(12,2) DEFAULT NULL,
  `num_num` decimal(12,3) DEFAULT '0.000',
  `num_back` decimal(12,3) DEFAULT '0.000',
  `vch_back_operID` varchar(6) DEFAULT NULL,
  `dt_back_operdate` datetime DEFAULT NULL,
  `int_discount` int(11) DEFAULT NULL,
  `vch_dis_operID` varchar(6) DEFAULT NULL,
  `dt_dis_operdate` datetime DEFAULT NULL,
  `ch_printflag` varchar(1) DEFAULT NULL,
  `vch_print_memo` varchar(1023) DEFAULT NULL,
  `ch_suitflag` varchar(1) DEFAULT NULL,
  `ch_suitno` varchar(7) DEFAULT NULL,
  `ch_specialflag` varchar(1) DEFAULT NULL,
  `ch_presentflag` varchar(1) DEFAULT NULL,
  `vch_pre_operID` varchar(6) DEFAULT NULL,
  `dt_pre_operdate` datetime DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `num_price_add` decimal(20,2) DEFAULT '0.00',
  `vch_plan_k3o3_id` varchar(12) DEFAULT NULL,
  `vch_plan_k3o3_state` varchar(6) DEFAULT NULL,
  `int_rate` decimal(12,2) DEFAULT '0.00',
  `ch_member_belong_state` varchar(1) DEFAULT NULL,
  `ch_addon_flag` varchar(1) DEFAULT NULL,
  `ch_addon_no` varchar(7) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`,`int_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_hungbill`
--

LOCK TABLES `cey_u_hungbill` WRITE;
/*!40000 ALTER TABLE `cey_u_hungbill` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_hungbill` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_master`
--

DROP TABLE IF EXISTS `cey_u_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_master` (
  `ch_billno` varchar(15) NOT NULL,
  `ch_state` varchar(1) NOT NULL DEFAULT '1',
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_operID2` varchar(6) DEFAULT NULL,
  `dt_operdate2` datetime DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `terminal_code` varchar(35) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT 'N',
  PRIMARY KEY (`ch_billno`),
  KEY `idx_cey_u_master_1` (`ch_billno`),
  KEY `idx_cey_u_master_3` (`dt_operdate`),
  KEY `idx_cey_u_master_4` (`ch_state`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_master`
--

LOCK TABLES `cey_u_master` WRITE;
/*!40000 ALTER TABLE `cey_u_master` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_master` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_master_help`
--

DROP TABLE IF EXISTS `cey_u_master_help`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_master_help` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `ch_billno` varchar(15) DEFAULT NULL,
  `date` date DEFAULT NULL,
  PRIMARY KEY (`int_flowID`),
  UNIQUE KEY `Idx_cey_u_master_help` (`ch_billno`),
  KEY `Idx_cey_u_master_help_2` (`date`)
) ENGINE=InnoDB AUTO_INCREMENT=228 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_master_help`
--

LOCK TABLES `cey_u_master_help` WRITE;
/*!40000 ALTER TABLE `cey_u_master_help` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_master_help` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_orderdish`
--

DROP TABLE IF EXISTS `cey_u_orderdish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_orderdish` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `ch_billno` varchar(15) NOT NULL,
  `ch_tableno` varchar(4) NOT NULL,
  `int_id` int(11) NOT NULL,
  `ch_dishno` varchar(7) DEFAULT NULL,
  `num_price` decimal(12,2) DEFAULT '0.00',
  `num_price_org` decimal(12,2) DEFAULT '0.00',
  `num_price_add` decimal(12,2) DEFAULT '0.00',
  `num_num` decimal(12,3) DEFAULT '0.000',
  `num_back` decimal(12,3) DEFAULT '0.000',
  `vch_back_operID` varchar(6) DEFAULT NULL,
  `dt_back_operdate` datetime DEFAULT NULL,
  `int_discount` int(11) DEFAULT NULL,
  `vch_dis_operID` varchar(6) DEFAULT NULL,
  `dt_dis_operdate` datetime DEFAULT NULL,
  `vch_memberno` varchar(20) DEFAULT NULL,
  `ch_consult` varchar(1) DEFAULT NULL,
  `ch_printflag` varchar(1) DEFAULT NULL,
  `vch_print_memo` varchar(1023) DEFAULT NULL,
  `ch_suitflag` varchar(1) DEFAULT NULL,
  `ch_suitno` varchar(7) DEFAULT NULL,
  `ch_specialflag` varchar(1) DEFAULT NULL,
  `ch_presentflag` varchar(1) DEFAULT NULL,
  `vch_pre_operID` varchar(6) DEFAULT NULL,
  `dt_pre_operdate` datetime DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_togono` varchar(15) DEFAULT NULL,
  `ch_payno` varchar(15) DEFAULT NULL,
  `ch_outflag` varchar(1) DEFAULT NULL,
  `vch_plan_k3o3_id` varchar(12) DEFAULT NULL,
  `vch_plan_k3o3_state` varchar(6) DEFAULT NULL,
  `vch_plan_desc` varchar(1023) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `vch_wx_serial` varchar(32) DEFAULT NULL,
  `int_rate` decimal(12,2) DEFAULT '0.00',
  `ch_member_belong_state` varchar(1) DEFAULT NULL,
  `ch_waidai_flag` varchar(1) DEFAULT '',
  `terminal_code` varchar(128) DEFAULT '',
  `vch_qr_code_sn` varchar(32) DEFAULT '',
  `ch_addon_flag` varchar(1) DEFAULT NULL,
  `ch_addon_no` varchar(7) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`),
  UNIQUE KEY `Idx_cey_u_orderdish` (`ch_billno`,`ch_tableno`,`int_id`),
  UNIQUE KEY `Idx_u_orderdish` (`ch_billno`,`ch_tableno`,`int_id`),
  KEY `idx_cey_u_orderdish_4` (`ch_dishno`),
  KEY `idx_cey_u_orderdish_5` (`dt_operdate`),
  KEY `idx_cey_u_orderdish_6` (`ch_payno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB AUTO_INCREMENT=35 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_orderdish`
--

LOCK TABLES `cey_u_orderdish` WRITE;
/*!40000 ALTER TABLE `cey_u_orderdish` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_orderdish` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER tr_cey_u_orderdish_insert AFTER INSERT ON cey_u_orderdish FOR EACH ROW 
   BEGIN      DECLARE flowId integer;
      DECLARE dishno varchar(7);
      DECLARE numnum numeric(10, 3);
      DECLARE materialno varchar(7);
      DECLARE intflowId integer;
      DECLARE initqty numeric(16,4);
      DECLARE initprice numeric(16,4);
      DECLARE remainqty numeric(16,4);
      DECLARE remainprice numeric(16,4);
      DECLARE itemflag char(1);
      DECLARE plan_id varchar(2);
      DECLARE kitchen_type varchar(32);
        set flowId = NEW.int_flowId;
        set dishno = NEW.ch_dishno;
        set numnum = NEW.num_num - NEW.num_back;
        set itemflag = 'N';
        set itemflag = (select ifnull(item_flag,'N') from cey_bt_dish where ch_dishno = dishno);
        set materialno = dishno;
        set kitchen_type = (select vch_value from cey_sys_parameter where vch_parameter = 'kt_kitchen_type');
        IF (kitchen_type = 'kitchen_print' ) THEN
        insert cey_u_orderdish_print_tr(int_orderflow, vch_plan_k3o3_id, vch_plan_k3o3_state, dt_operdate, ch_billno, ch_dishno) values (flowId, NEW.vch_plan_k3o3_id, NEW.vch_plan_k3o3_state, NEW.dt_operdate, NEW.ch_billno, NEW.ch_dishno);
        END IF;
        IF (kitchen_type = 'kitchen_view' ) THEN
               set plan_id = mid(NEW.vch_plan_k3o3_id, 1, 2);
               IF  exists (select 1 from cey_bt_kitchen_view  where vch_plan_id = plan_id) THEN
                           insert into cey_u_orderdish_print_kt_order(int_flowID,vch_plan_id,ch_billno, ch_dishno, ch_state) values (flowId,plan_id, NEW.ch_billno,NEW.ch_dishno, 0);
                           IF not exists (select 1 from cey_u_orderdish_print_kt_table  where vch_plan_id = plan_id and ch_billno = NEW.ch_billno) THEN
                                              insert into cey_u_orderdish_print_kt_table(vch_plan_id,ch_billno) values (plan_id, NEW.ch_billno);
                           END IF;
                           update cey_u_orderdish_print_kt_table set ch_state = 0, dt_operdate = now() where vch_plan_id = plan_id and ch_billno = NEW.ch_billno;
                           IF not exists (select 1 from cey_u_orderdish_print_kt_dish  where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno) THEN
                                              insert into cey_u_orderdish_print_kt_dish(vch_plan_id, ch_dishno) values (plan_id, NEW.ch_dishno);
                           END IF;
                           update cey_u_orderdish_print_kt_dish set num_num = num_num + NEW.num_num where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno;
                           update cey_u_orderdish_print_kt_dish set num_back = num_back + NEW.num_back where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno;
                           update cey_bt_kitchen_view set ch_refresh_order_insert = 'Y' where vch_plan_id = plan_id;
               END IF;
        END IF;
       IF (materialno<>'' and itemflag = 'Y') THEN         set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow where ch_materialno = materialno);
      IF (intflowId = 0) THEN               set initqty = 0;
           ELSE               set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);
       END IF;
      IF (intflowId = 0) THEN         set initprice = 0;
         ELSE           set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);
       END IF;
    set remainqty  = initqty - numnum ;
       IF (remainqty = 0) THEN          set remainprice = 0;
       ELSE                set remainprice = initprice;
        END IF;
       insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,               num_num, sheet_price, cost_price, remain_qty, remain_price, ch_sheetno )          values (materialno,now(), 'QT', if(numnum < 0, -1, 1), abs(numnum), 0, remainprice, remainqty, remainprice, LEFT(flowId,20));
      IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN       update cey_st_material_num set num_num = remainqty, cost_price = remainprice  where ch_materialno = materialno;
       ELSE         insert into cey_st_material_num (ch_materialno, num_num, cost_price) values (materialno, remainqty, remainprice);
       END IF;
      END IF;
    END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER tr_cey_u_orderdish_update AFTER UPDATE ON cey_u_orderdish FOR EACH ROW 
  BEGIN    DECLARE m_done int default 0;
    DECLARE flowId integer;
    DECLARE dishno varchar(7);
    DECLARE numnum numeric(10, 3);
    DECLARE materialno varchar(7);
    DECLARE intflowId integer;
    DECLARE initqty numeric(16,4);
    DECLARE initprice numeric(16,4);
    DECLARE remainqty numeric(16,4);
    DECLARE remainprice numeric(16,4);
    DECLARE itemflag char(1);
    DECLARE kitchen_type varchar(32);
    DECLARE plan_id varchar(2);
    set kitchen_type = (select vch_value from cey_sys_parameter where vch_parameter = 'kt_kitchen_type');
    set flowId = NEW.int_flowId;
    IF(NEW.int_flowId = OLD.int_flowId and kitchen_type = 'kitchen_print'  and NEW.vch_plan_k3o3_state <> OLD.vch_plan_k3o3_state) THEN
             update cey_u_orderdish_print_tr set vch_plan_k3o3_state = NEW.vch_plan_k3o3_state where int_orderflow = flowId;
    END IF;
    IF (NEW.int_flowId = OLD.int_flowId and kitchen_type = 'kitchen_view' ) THEN
             set plan_id = mid(OLD.vch_plan_k3o3_id, 1, 2);
             IF  exists (select 1 from cey_bt_kitchen_view  where vch_plan_id = plan_id) THEN
                      IF (NEW.num_back <> OLD.num_back or NEW.num_num <> OLD.num_num ) THEN
                               update cey_u_orderdish_print_kt_dish set num_num = num_num - (OLD.num_num - NEW.num_num) where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno;
                               update cey_u_orderdish_print_kt_dish set num_back = num_back - (OLD.num_back - NEW.num_back) where vch_plan_id = plan_id and ch_dishno = NEW.ch_dishno;
                               update cey_bt_kitchen_view set ch_refresh_order_update = 'Y' where vch_plan_id = plan_id;
                      END IF;
             END IF;
    END IF;
   IF(NEW.int_flowId = OLD.int_flowId and (NEW.num_back <> OLD.num_back or NEW.num_num <> OLD.num_num)) THEN
    set dishno = NEW.ch_dishno;
      set numnum = OLD.num_num - OLD.num_back - (NEW.num_num - NEW.num_back);
    set itemflag = 'N';
    set itemflag = (select ifnull(item_flag,'N') from cey_bt_dish where ch_dishno = dishno);
    set materialno = dishno;
    IF (materialno<>'' and itemflag = 'Y')  THEN
         set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow where ch_materialno = materialno);
         IF (intflowId = 0) THEN
              set initqty = 0;
         ELSE
              set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);
         END IF;
         IF (intflowId = 0) THEN
              set initprice = 0;
         ELSE
              set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);
         END IF;
         set remainqty = initqty + numnum ;
         IF (remainqty = 0)  THEN
              set remainprice = 0;
         ELSE
              set remainprice = initprice;
         END IF;
         insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
                         num_num, sheet_price, cost_price, remain_qty, remain_price, ch_sheetno )
                 values (materialno,now(), 'QT', if(numnum < 0, -1, 1), abs(numnum), 0, remainprice, remainqty, remainprice, left(flowId,20));
        IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN
                 update cey_st_material_num set num_num = remainqty, cost_price = remainprice
                  where ch_materialno = materialno;
        ELSE
                 insert into cey_st_material_num (ch_materialno, num_num, cost_price)
                 values (materialno, remainqty, remainprice);
        END IF;
     END IF;
       END IF;
 END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `cey_u_orderdish_log`
--

DROP TABLE IF EXISTS `cey_u_orderdish_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_orderdish_log` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `ch_type` varchar(4) NOT NULL,
  `vch_impower` varchar(6) DEFAULT NULL,
  `vch_reason` varchar(60) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_memberno` varchar(30) DEFAULT NULL,
  `vch_memo` varchar(40) DEFAULT NULL,
  `int_orderflow` int(11) DEFAULT NULL,
  `num_back_cur` decimal(12,3) DEFAULT '0.000',
  `vch_plan_k3o3_state` varchar(6) DEFAULT NULL,
  `vch_print_memo` varchar(1023) DEFAULT NULL,
  `num_price_add` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `ch_payno` varchar(15) DEFAULT NULL,
  `terminal_code` varchar(128) DEFAULT '',
  PRIMARY KEY (`int_flowID`),
  KEY `index_name` (`ch_yun_already_keep`),
  KEY `idx_int_orderflow` (`int_orderflow`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_orderdish_log`
--

LOCK TABLES `cey_u_orderdish_log` WRITE;
/*!40000 ALTER TABLE `cey_u_orderdish_log` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_orderdish_log` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_orderdish_print_kt_dish`
--

DROP TABLE IF EXISTS `cey_u_orderdish_print_kt_dish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_orderdish_print_kt_dish` (
  `vch_plan_id` varchar(2) NOT NULL,
  `ch_dishno` varchar(7) NOT NULL,
  `ch_billno` varchar(15) NOT NULL,
  `num_num` decimal(12,3) DEFAULT '0.000',
  `num_back` decimal(12,3) DEFAULT '0.000',
  `num_cook` decimal(12,3) DEFAULT '0.000',
  `num_finish` decimal(12,3) DEFAULT '0.000',
  PRIMARY KEY (`vch_plan_id`,`ch_dishno`),
  KEY `idx_ch_billno` (`ch_billno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_orderdish_print_kt_dish`
--

LOCK TABLES `cey_u_orderdish_print_kt_dish` WRITE;
/*!40000 ALTER TABLE `cey_u_orderdish_print_kt_dish` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_orderdish_print_kt_dish` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_orderdish_print_kt_order`
--

DROP TABLE IF EXISTS `cey_u_orderdish_print_kt_order`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_orderdish_print_kt_order` (
  `int_flowID` int(11) NOT NULL,
  `vch_plan_id` varchar(2) DEFAULT NULL,
  `ch_billno` varchar(15) DEFAULT NULL,
  `ch_dishno` varchar(7) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`),
  KEY `idx_vch_plan_id` (`vch_plan_id`),
  KEY `idx_ch_billno` (`ch_billno`),
  KEY `idx_ch_dishno` (`ch_dishno`),
  KEY `idx_ch_state` (`ch_state`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_orderdish_print_kt_order`
--

LOCK TABLES `cey_u_orderdish_print_kt_order` WRITE;
/*!40000 ALTER TABLE `cey_u_orderdish_print_kt_order` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_orderdish_print_kt_order` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_orderdish_print_kt_quit_queue`
--

DROP TABLE IF EXISTS `cey_u_orderdish_print_kt_quit_queue`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_orderdish_print_kt_quit_queue` (
  `int_orderflow` int(11) NOT NULL,
  `vch_plan_id` varchar(2) NOT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `vch_back_operID` varchar(6) DEFAULT NULL,
  `vch_print_memo` varchar(1023) DEFAULT NULL,
  `num_back` decimal(12,3) DEFAULT NULL,
  PRIMARY KEY (`int_orderflow`),
  KEY `idx_vch_plan_id` (`vch_plan_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_orderdish_print_kt_quit_queue`
--

LOCK TABLES `cey_u_orderdish_print_kt_quit_queue` WRITE;
/*!40000 ALTER TABLE `cey_u_orderdish_print_kt_quit_queue` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_orderdish_print_kt_quit_queue` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_orderdish_print_kt_table`
--

DROP TABLE IF EXISTS `cey_u_orderdish_print_kt_table`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_orderdish_print_kt_table` (
  `vch_plan_id` varchar(2) NOT NULL,
  `ch_billno` varchar(15) NOT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `int_cook` int(11) DEFAULT '0',
  `int_finish` int(11) DEFAULT '0',
  PRIMARY KEY (`vch_plan_id`,`ch_billno`),
  KEY `idx_ch_state` (`ch_state`),
  KEY `idx_dt_operdate` (`dt_operdate`),
  KEY `idx_int_finish` (`int_finish`),
  KEY `idx_int_cook` (`int_cook`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_orderdish_print_kt_table`
--

LOCK TABLES `cey_u_orderdish_print_kt_table` WRITE;
/*!40000 ALTER TABLE `cey_u_orderdish_print_kt_table` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_orderdish_print_kt_table` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_orderdish_print_quit`
--

DROP TABLE IF EXISTS `cey_u_orderdish_print_quit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_orderdish_print_quit` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT COMMENT '流水号',
  `int_orderflow` int(11) NOT NULL,
  `vch_plan_k3o3_state` varchar(6) DEFAULT NULL,
  `num_back_cur` decimal(12,2) DEFAULT NULL,
  `vch_print_memo` varchar(1023) DEFAULT NULL,
  `num_price_add` decimal(12,2) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_orderdish_print_quit`
--

LOCK TABLES `cey_u_orderdish_print_quit` WRITE;
/*!40000 ALTER TABLE `cey_u_orderdish_print_quit` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_orderdish_print_quit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_orderdish_print_tr`
--

DROP TABLE IF EXISTS `cey_u_orderdish_print_tr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_orderdish_print_tr` (
  `int_orderflow` int(11) NOT NULL,
  `vch_plan_k3o3_id` varchar(12) DEFAULT NULL,
  `vch_plan_k3o3_state` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_billno` varchar(15) DEFAULT NULL,
  `ch_dishno` varchar(7) DEFAULT NULL,
  PRIMARY KEY (`int_orderflow`),
  KEY `idx_cey_u_orderdish_print_tr_2` (`ch_billno`),
  KEY `idx_cey_u_orderdish_print_tr_3` (`dt_operdate`),
  KEY `index_name` (`vch_plan_k3o3_state`),
  KEY `idx_cey_u_orderdish_print_tr_4` (`vch_plan_k3o3_state`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_orderdish_print_tr`
--

LOCK TABLES `cey_u_orderdish_print_tr` WRITE;
/*!40000 ALTER TABLE `cey_u_orderdish_print_tr` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_orderdish_print_tr` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_service_low`
--

DROP TABLE IF EXISTS `cey_u_service_low`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_service_low` (
  `ch_payno` varchar(15) NOT NULL,
  `ch_type` varchar(1) NOT NULL,
  `num_amount` decimal(12,2) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `terminal_code` varchar(128) DEFAULT '',
  PRIMARY KEY (`ch_payno`,`ch_type`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_service_low`
--

LOCK TABLES `cey_u_service_low` WRITE;
/*!40000 ALTER TABLE `cey_u_service_low` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_service_low` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_table`
--

DROP TABLE IF EXISTS `cey_u_table`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_table` (
  `ch_billno` varchar(15) NOT NULL,
  `ch_tableno` varchar(4) NOT NULL,
  `int_person` int(11) DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT '1',
  `ch_payno` varchar(15) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_operID2` varchar(6) DEFAULT NULL,
  `dt_operdate2` datetime DEFAULT NULL,
  `ch_order_state` varchar(1) DEFAULT '0',
  `num_cost` decimal(12,2) DEFAULT NULL,
  `vch_waiter` varchar(6) DEFAULT NULL,
  `num_trans_service` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `terminal_code` varchar(128) DEFAULT '',
  `vch_qr_code_sn` varchar(32) DEFAULT '',
  PRIMARY KEY (`ch_billno`),
  KEY `idx_cey_u_table_1` (`ch_billno`),
  KEY `idx_cey_u_table_2` (`ch_tableno`),
  KEY `idx_cey_u_table_3` (`dt_operdate`),
  KEY `idx_cey_u_table_4` (`ch_payno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_table`
--

LOCK TABLES `cey_u_table` WRITE;
/*!40000 ALTER TABLE `cey_u_table` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_table` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_taxsale_master`
--

DROP TABLE IF EXISTS `cey_u_taxsale_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_taxsale_master` (
  `ch_billno` varchar(15) NOT NULL,
  `ch_payno` varchar(15) DEFAULT NULL,
  `ch_tax_mode` varchar(1) DEFAULT NULL,
  `ch_tax_type` varchar(1) DEFAULT NULL,
  `num_before` decimal(10,2) DEFAULT '0.00' COMMENT '税前金额',
  `num_cur` decimal(10,2) DEFAULT '0.00' COMMENT '税1',
  `num_cur_2` decimal(10,2) DEFAULT '0.00',
  `num_cur_3` decimal(10,2) DEFAULT '0.00',
  `ch_fix_flag` varchar(1) DEFAULT NULL COMMENT '是否固定税率',
  `int_fix_tax` decimal(10,2) DEFAULT '0.00' COMMENT '固定税率1',
  `int_fix_tax_2` decimal(10,2) DEFAULT '0.00',
  `int_fix_tax_3` decimal(10,2) DEFAULT '0.00',
  `ch_fix_service` varchar(1) DEFAULT NULL COMMENT '是否固定服务费',
  `int_fix_service` decimal(10,2) DEFAULT '0.00' COMMENT '固定服务费',
  `dt_operdate` datetime DEFAULT NULL,
  PRIMARY KEY (`ch_billno`),
  KEY `idx_cey_u_taxsale_master_1` (`dt_operdate`),
  KEY `idx_cey_u_taxsale_master_2` (`ch_payno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_taxsale_master`
--

LOCK TABLES `cey_u_taxsale_master` WRITE;
/*!40000 ALTER TABLE `cey_u_taxsale_master` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_taxsale_master` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_togo`
--

DROP TABLE IF EXISTS `cey_u_togo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_togo` (
  `ch_togono` varchar(15) NOT NULL,
  `ch_billno` varchar(15) NOT NULL,
  `ch_payno` varchar(15) DEFAULT NULL,
  `num_cost` decimal(12,2) DEFAULT NULL,
  `num_discount` decimal(12,2) DEFAULT NULL,
  `num_present` decimal(12,2) DEFAULT NULL,
  `num_blotout` decimal(12,2) DEFAULT NULL,
  `vch_waiter` varchar(6) DEFAULT NULL,
  `vch_address` varchar(50) DEFAULT NULL,
  `vch_linkman` varchar(20) DEFAULT NULL,
  `vch_tel` varchar(15) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `num_rate` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `vch_yun_sn` varchar(15) DEFAULT NULL,
  `terminal_code` varchar(128) DEFAULT '',
  PRIMARY KEY (`ch_togono`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_togo`
--

LOCK TABLES `cey_u_togo` WRITE;
/*!40000 ALTER TABLE `cey_u_togo` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_togo` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_togo_guest`
--

DROP TABLE IF EXISTS `cey_u_togo_guest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_togo_guest` (
  `ch_guestid` varchar(15) NOT NULL,
  `vch_guestname` varchar(20) NOT NULL,
  `vch_address` varchar(50) DEFAULT NULL,
  `vch_tel` varchar(15) DEFAULT NULL,
  PRIMARY KEY (`ch_guestid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_togo_guest`
--

LOCK TABLES `cey_u_togo_guest` WRITE;
/*!40000 ALTER TABLE `cey_u_togo_guest` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_togo_guest` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_u_uncheckout_log`
--

DROP TABLE IF EXISTS `cey_u_uncheckout_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_u_uncheckout_log` (
  `int_flowid` int(11) NOT NULL AUTO_INCREMENT,
  `ch_billno` varchar(15) DEFAULT NULL,
  `ch_payno_un` varchar(15) DEFAULT NULL,
  `num_cost_un` decimal(12,2) DEFAULT NULL,
  `vch_operid` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  PRIMARY KEY (`int_flowid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_u_uncheckout_log`
--

LOCK TABLES `cey_u_uncheckout_log` WRITE;
/*!40000 ALTER TABLE `cey_u_uncheckout_log` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_u_uncheckout_log` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_web_data_collection`
--

DROP TABLE IF EXISTS `cey_web_data_collection`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_web_data_collection` (
  `id` varchar(10) NOT NULL,
  `service_ip` varchar(100) NOT NULL,
  `service_port` varchar(100) NOT NULL,
  `service_action` varchar(100) NOT NULL,
  `service_method_name` varchar(100) NOT NULL,
  `service_method_uri` varchar(100) NOT NULL,
  `service_submit_path` varchar(100) NOT NULL,
  `username` varchar(20) NOT NULL,
  `password` varchar(20) DEFAULT NULL,
  `messagetype` varchar(20) NOT NULL,
  `messageid` varchar(20) NOT NULL,
  `version` varchar(20) NOT NULL,
  `txdate_yyyymmdd` date NOT NULL,
  `txtime_hhmmss` time NOT NULL,
  `mallid` varchar(20) NOT NULL,
  `storecode` varchar(20) NOT NULL,
  `tillid` varchar(20) DEFAULT NULL,
  `txdocno` varchar(20) NOT NULL,
  `cashier` varchar(20) DEFAULT NULL,
  `salesman` varchar(20) DEFAULT NULL,
  `itemcode` varchar(20) NOT NULL,
  `qty` decimal(10,2) NOT NULL,
  `netamount` decimal(10,2) NOT NULL,
  `ch_uploadflag` varchar(1) DEFAULT NULL,
  `ch_uploadmemo` varchar(100) DEFAULT NULL,
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_web_data_collection`
--

LOCK TABLES `cey_web_data_collection` WRITE;
/*!40000 ALTER TABLE `cey_web_data_collection` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_web_data_collection` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_web_data_collection_tendercode`
--

DROP TABLE IF EXISTS `cey_web_data_collection_tendercode`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_web_data_collection_tendercode` (
  `id` varchar(10) NOT NULL,
  `tendercode` varchar(10) NOT NULL,
  `baseamount` decimal(10,2) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_web_data_collection_tendercode`
--

LOCK TABLES `cey_web_data_collection_tendercode` WRITE;
/*!40000 ALTER TABLE `cey_web_data_collection_tendercode` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_web_data_collection_tendercode` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_web_order_dish`
--

DROP TABLE IF EXISTS `cey_web_order_dish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_web_order_dish` (
  `sn` float NOT NULL COMMENT '流水号',
  `orderSn` float DEFAULT NULL COMMENT '订单编号',
  `dishNo` varchar(255) DEFAULT NULL COMMENT '菜品编号',
  `dishName` varchar(255) DEFAULT NULL COMMENT '菜品名称',
  `number` float DEFAULT NULL COMMENT '数量',
  `price` float DEFAULT NULL COMMENT '价格',
  `suitFlag` decimal(10,0) DEFAULT NULL COMMENT '套菜标志位\r\n0：不是套菜\r\n1：是套菜',
  `note` varchar(255) DEFAULT NULL COMMENT '备注',
  `orgCode` varchar(255) DEFAULT NULL COMMENT 'orgCode',
  `date` datetime DEFAULT NULL COMMENT '插入表时间',
  `downFlag` decimal(10,0) DEFAULT NULL COMMENT '下发状态位\r\n0：未下发\r\n1：已经下发',
  PRIMARY KEY (`sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_web_order_dish`
--

LOCK TABLES `cey_web_order_dish` WRITE;
/*!40000 ALTER TABLE `cey_web_order_dish` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_web_order_dish` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_web_order_info`
--

DROP TABLE IF EXISTS `cey_web_order_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_web_order_info` (
  `sn` float NOT NULL COMMENT '订单编号',
  `eatTime` datetime DEFAULT NULL COMMENT '就餐时间',
  `eatPels` float DEFAULT NULL COMMENT '就餐人数',
  `eatTableNo` varchar(64) DEFAULT NULL COMMENT '餐桌编号',
  `tableAreaNo` varchar(64) DEFAULT NULL COMMENT '餐区编号',
  `tel` varchar(64) DEFAULT NULL COMMENT '手机号码',
  `orderer` varchar(64) DEFAULT NULL COMMENT '预订人',
  `sex` varchar(64) DEFAULT NULL COMMENT '性别',
  `note` varchar(64) DEFAULT NULL COMMENT '订单备注',
  `address` varchar(255) DEFAULT NULL COMMENT '地址',
  `sendTime` datetime DEFAULT NULL COMMENT '送达时间',
  `payType` varchar(64) DEFAULT NULL COMMENT '支付方式',
  `payState` varchar(64) DEFAULT NULL COMMENT '支付状态',
  `orderType` varchar(64) DEFAULT NULL COMMENT '订单类型',
  `orderState` varchar(64) DEFAULT NULL COMMENT '订单状态',
  `openid` varchar(128) DEFAULT NULL,
  `orgCode` varchar(64) DEFAULT NULL,
  `date` datetime DEFAULT NULL,
  `refuseReason` varchar(255) DEFAULT NULL COMMENT '订桌，外卖，拒绝理由',
  `downFlag` decimal(10,0) DEFAULT NULL COMMENT '下发状态位',
  PRIMARY KEY (`sn`),
  KEY `sn` (`sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_web_order_info`
--

LOCK TABLES `cey_web_order_info` WRITE;
/*!40000 ALTER TABLE `cey_web_order_info` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_web_order_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_web_pay_for_water`
--

DROP TABLE IF EXISTS `cey_web_pay_for_water`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_web_pay_for_water` (
  `sn` float NOT NULL COMMENT '支付编号',
  `orderSn` float NOT NULL COMMENT '订桌、外卖编号',
  `mount` float DEFAULT NULL COMMENT '金额',
  `note` varchar(255) DEFAULT NULL COMMENT '备注',
  `openid` varchar(128) DEFAULT NULL,
  `orgCode` varchar(64) DEFAULT NULL,
  `date` datetime DEFAULT NULL COMMENT '插入表时间',
  `upLoadFlag` decimal(10,0) DEFAULT NULL COMMENT '客户机上传状态位（服务器端可忽略）\r\n0：未上传\r\n1：已上传',
  PRIMARY KEY (`sn`),
  KEY `paySn` (`sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_web_pay_for_water`
--

LOCK TABLES `cey_web_pay_for_water` WRITE;
/*!40000 ALTER TABLE `cey_web_pay_for_water` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_web_pay_for_water` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_web_table_delrecord`
--

DROP TABLE IF EXISTS `cey_web_table_delrecord`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_web_table_delrecord` (
  `vch_sqltablename` varchar(30) NOT NULL,
  `vch_keyname_value` varchar(30) NOT NULL,
  `vch_keyname` varchar(30) NOT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`vch_sqltablename`,`vch_keyname_value`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_web_table_delrecord`
--

LOCK TABLES `cey_web_table_delrecord` WRITE;
/*!40000 ALTER TABLE `cey_web_table_delrecord` DISABLE KEYS */;
INSERT INTO `cey_web_table_delrecord` VALUES ('cey_bt_cp_dish','10101,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10101,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10101,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10101,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10101,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10111,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10111,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10111,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10111,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10111,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10112,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10112,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10112,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10112,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10112,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10113,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10113,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10113,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10113,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10113,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10114,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10114,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10114,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10114,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10114,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10115,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10115,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10115,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10115,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10115,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10116,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10116,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10116,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10116,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10116,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10117,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10117,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10117,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10117,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10117,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10118,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10118,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10118,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10118,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10118,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10119,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10119,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10119,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10119,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10119,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10120,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10120,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10120,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10120,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10120,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10121,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10121,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10121,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10121,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10121,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10122,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10122,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10122,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10122,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10122,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10123,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10123,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10123,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10123,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10123,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10124,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10124,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10124,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10124,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10124,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10125,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10125,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10125,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10125,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10125,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10126,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10126,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10126,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10126,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10126,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10127,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10127,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10127,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10127,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10127,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10128,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10128,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10128,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10128,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10128,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10130,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10130,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10130,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10130,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10130,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10131,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10131,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10131,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10131,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10131,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10134,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10134,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10134,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10134,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10134,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10135,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10135,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10135,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10135,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10135,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10136,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10136,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10136,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10136,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10136,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10137,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10137,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10137,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10137,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10137,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10138,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10138,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10138,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10138,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10138,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10139,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10139,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10139,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10139,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10139,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10140,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10140,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10140,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10140,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10140,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10141,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10141,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10141,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10141,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10141,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10142,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10142,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10142,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10142,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10142,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10143,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10143,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10143,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10143,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10143,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10144,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10144,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10144,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10144,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10144,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10145,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10145,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10145,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10145,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10145,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10146,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10146,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10146,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10146,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10146,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10147,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10147,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10147,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10147,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10147,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10148,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10148,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10148,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10148,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10148,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10149,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10149,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10149,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10149,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10149,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10150,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10150,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10150,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10150,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10150,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10151,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10151,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10151,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10151,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10151,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10152,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10152,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10152,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10152,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10152,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10153,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10153,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10153,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10153,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10153,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10154,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10154,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10154,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10154,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10154,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10155,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10155,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10155,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10155,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10155,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10156,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10156,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10156,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10156,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10156,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10157,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10157,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10157,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10157,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10157,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10158,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10158,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10158,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10158,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10158,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10159,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10159,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10159,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10159,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10159,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10160,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10160,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10160,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10160,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10160,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10161,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10161,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10161,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10161,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10161,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10162,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10162,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10162,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10162,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10162,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10163,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10163,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10163,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10163,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10163,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10164,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10164,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10164,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10164,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10164,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10165,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10165,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10165,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10165,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10165,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10166,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10166,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10166,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10166,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10166,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10167,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10167,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10167,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10167,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10167,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10168,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10168,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10168,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10168,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10168,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10169,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10169,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10169,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10169,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10169,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10170,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10170,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10170,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10170,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10170,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10171,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10171,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10171,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10171,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10171,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10172,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10172,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10172,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10172,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10172,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10173,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10173,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10173,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10173,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10173,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10174,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10174,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10174,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10174,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10174,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10175,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10175,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10175,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10175,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10175,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10176,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10176,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10176,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10176,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10176,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10177,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10177,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10177,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10177,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10177,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10180,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10180,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10180,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10180,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10180,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10181,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10181,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10181,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10181,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10181,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10182,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10182,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10182,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10182,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10182,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10183,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10183,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10183,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10183,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10183,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10184,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10184,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10184,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10184,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10184,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10185,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10185,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10185,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10185,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10185,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10186,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10186,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10186,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10186,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10186,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10187,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10187,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10187,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10187,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10187,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10188,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10188,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10188,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10188,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10188,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10190,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10190,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10190,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10190,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10190,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10191,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10191,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10191,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10191,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10191,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10192,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10192,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10192,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10192,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10192,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10193,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10193,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10193,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10193,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10193,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10194,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10194,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10194,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10194,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10194,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10195,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10195,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10195,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10195,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10195,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10196,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10196,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10196,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10196,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10196,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10197,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10197,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10197,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10197,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10197,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10198,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10198,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10198,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10198,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10198,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10199,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10199,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10199,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10199,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10199,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10200,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10200,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10200,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10200,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10200,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10201,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10201,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10201,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10201,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10201,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10202,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10202,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10202,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10202,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10202,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10203,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10203,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10203,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10203,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10203,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10204,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10204,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10204,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10204,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10204,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10205,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10205,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10205,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10205,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10205,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10206,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10206,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10206,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10206,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10206,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10207,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10207,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10207,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10207,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10207,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10208,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10208,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10208,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10208,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10208,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10209,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10209,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10209,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10209,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10209,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10210,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10210,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10210,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10210,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10210,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10211,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10211,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10211,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10211,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','10211,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11001,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11001,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11001,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11001,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11001,015','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11002,010','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11002,011','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11002,012','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11002,013','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11002,014','ch_dishno,ch_no',NULL),('cey_bt_cp_dish','11002,015','ch_dishno,ch_no',NULL),('cey_bt_dish','10101','ch_dishno',NULL),('cey_bt_dish','10111','ch_dishno',NULL),('cey_bt_dish','10112','ch_dishno',NULL),('cey_bt_dish','10113','ch_dishno',NULL),('cey_bt_dish','10114','ch_dishno',NULL),('cey_bt_dish','10115','ch_dishno',NULL),('cey_bt_dish','10116','ch_dishno',NULL),('cey_bt_dish','10117','ch_dishno',NULL),('cey_bt_dish','10118','ch_dishno',NULL),('cey_bt_dish','10119','ch_dishno',NULL),('cey_bt_dish','10120','ch_dishno',NULL),('cey_bt_dish','10121','ch_dishno',NULL),('cey_bt_dish','10122','ch_dishno',NULL),('cey_bt_dish','10123','ch_dishno',NULL),('cey_bt_dish','10124','ch_dishno',NULL),('cey_bt_dish','10125','ch_dishno',NULL),('cey_bt_dish','10126','ch_dishno',NULL),('cey_bt_dish','10127','ch_dishno',NULL),('cey_bt_dish','10128','ch_dishno',NULL),('cey_bt_dish','10130','ch_dishno',NULL),('cey_bt_dish','10131','ch_dishno',NULL),('cey_bt_dish','10134','ch_dishno',NULL),('cey_bt_dish','10135','ch_dishno',NULL),('cey_bt_dish','10136','ch_dishno',NULL),('cey_bt_dish','10137','ch_dishno',NULL),('cey_bt_dish','10138','ch_dishno',NULL),('cey_bt_dish','10139','ch_dishno',NULL),('cey_bt_dish','10140','ch_dishno',NULL),('cey_bt_dish','10141','ch_dishno',NULL),('cey_bt_dish','10142','ch_dishno',NULL),('cey_bt_dish','10143','ch_dishno',NULL),('cey_bt_dish','10144','ch_dishno',NULL),('cey_bt_dish','10145','ch_dishno',NULL),('cey_bt_dish','10146','ch_dishno',NULL),('cey_bt_dish','10147','ch_dishno',NULL),('cey_bt_dish','10148','ch_dishno',NULL),('cey_bt_dish','10149','ch_dishno',NULL),('cey_bt_dish','10150','ch_dishno',NULL),('cey_bt_dish','10151','ch_dishno',NULL),('cey_bt_dish','10152','ch_dishno',NULL),('cey_bt_dish','10153','ch_dishno',NULL),('cey_bt_dish','10154','ch_dishno',NULL),('cey_bt_dish','10155','ch_dishno',NULL),('cey_bt_dish','10156','ch_dishno',NULL),('cey_bt_dish','10157','ch_dishno',NULL),('cey_bt_dish','10158','ch_dishno',NULL),('cey_bt_dish','10159','ch_dishno',NULL),('cey_bt_dish','10160','ch_dishno',NULL),('cey_bt_dish','10161','ch_dishno',NULL),('cey_bt_dish','10162','ch_dishno',NULL),('cey_bt_dish','10163','ch_dishno',NULL),('cey_bt_dish','10164','ch_dishno',NULL),('cey_bt_dish','10165','ch_dishno',NULL),('cey_bt_dish','10166','ch_dishno',NULL),('cey_bt_dish','10167','ch_dishno',NULL),('cey_bt_dish','10168','ch_dishno',NULL),('cey_bt_dish','10169','ch_dishno',NULL),('cey_bt_dish','10170','ch_dishno',NULL),('cey_bt_dish','10171','ch_dishno',NULL),('cey_bt_dish','10172','ch_dishno',NULL),('cey_bt_dish','10173','ch_dishno',NULL),('cey_bt_dish','10174','ch_dishno',NULL),('cey_bt_dish','10175','ch_dishno',NULL),('cey_bt_dish','10176','ch_dishno',NULL),('cey_bt_dish','10177','ch_dishno',NULL),('cey_bt_dish','10180','ch_dishno',NULL),('cey_bt_dish','10181','ch_dishno',NULL),('cey_bt_dish','10182','ch_dishno',NULL),('cey_bt_dish','10183','ch_dishno',NULL),('cey_bt_dish','10184','ch_dishno',NULL),('cey_bt_dish','10185','ch_dishno',NULL),('cey_bt_dish','10186','ch_dishno',NULL),('cey_bt_dish','10187','ch_dishno',NULL),('cey_bt_dish','10188','ch_dishno',NULL),('cey_bt_dish','10190','ch_dishno',NULL),('cey_bt_dish','10191','ch_dishno',NULL),('cey_bt_dish','10192','ch_dishno',NULL),('cey_bt_dish','10193','ch_dishno',NULL),('cey_bt_dish','10194','ch_dishno',NULL),('cey_bt_dish','10195','ch_dishno',NULL),('cey_bt_dish','10196','ch_dishno',NULL),('cey_bt_dish','10197','ch_dishno',NULL),('cey_bt_dish','10198','ch_dishno',NULL),('cey_bt_dish','10199','ch_dishno',NULL),('cey_bt_dish','10200','ch_dishno',NULL),('cey_bt_dish','10201','ch_dishno',NULL),('cey_bt_dish','10202','ch_dishno',NULL),('cey_bt_dish','10203','ch_dishno',NULL),('cey_bt_dish','10204','ch_dishno',NULL),('cey_bt_dish','10205','ch_dishno',NULL),('cey_bt_dish','10206','ch_dishno',NULL),('cey_bt_dish','10207','ch_dishno',NULL),('cey_bt_dish','10208','ch_dishno',NULL),('cey_bt_dish','10209','ch_dishno',NULL),('cey_bt_dish','10210','ch_dishno',NULL),('cey_bt_dish','10211','ch_dishno',NULL),('cey_bt_dish','11001','ch_dishno',NULL),('cey_bt_dish','11002','ch_dishno',NULL),('cey_bt_dish_type','17','ch_dish_typeno',NULL),('cey_bt_unit','13','ch_unitno','Y');
/*!40000 ALTER TABLE `cey_web_table_delrecord` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_web_table_version`
--

DROP TABLE IF EXISTS `cey_web_table_version`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_web_table_version` (
  `vch_sqltablename` varchar(30) NOT NULL,
  `vch_sqltablename_key_name` varchar(30) NOT NULL,
  `vch_sqltablename_yun` varchar(30) NOT NULL,
  `vch_sqltablename_yun_key_name` varchar(30) NOT NULL,
  `vch_sqltablename_yun_version` varchar(30) NOT NULL,
  `int_version` int(10) NOT NULL,
  PRIMARY KEY (`vch_sqltablename`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_web_table_version`
--

LOCK TABLES `cey_web_table_version` WRITE;
/*!40000 ALTER TABLE `cey_web_table_version` DISABLE KEYS */;
INSERT INTO `cey_web_table_version` VALUES ('cey_bt_cp_dish','ch_dishno,ch_no','dishPracticeList','dpChDishNo,dpChNo','dishPracticeVersion',0),('cey_bt_cp_memo','ch_no','dishPracticeMemoList','chNo','dishPracticeMemoVersion',0),('cey_bt_cp_type','ch_typeno','dishPracticeTypeList','dptChTypeNo','dishPracticeTypeVersion',0),('cey_bt_dish','ch_dishno','dishInfoList','diChDishNo','dishInfoVersion',0),('cey_bt_dish_subtype','ch_sub_typeno','dishSubTypeList','dstChSubTypeNo','dishSubTypeVersion',0),('cey_bt_dish_suit','int_flowid','dishSuitList','dsIntFlowId','dishSuitVersion',0),('cey_bt_dish_type','ch_dish_typeno','dishTypeList','dtChDishTypeNo','dishTypeVersion',0),('cey_bt_dish_warn','int_flowid','dishGuQingList','dgqIntFlowId','dishGuQingVersion',0),('cey_bt_paymode','ch_paymodeno','payModeList','pmChPayModeNo','payModeVersion',0),('cey_bt_suit_change','int_flowid,ch_dishno','dishSuitChangeList','dscIntFlowId,dscChDishNo','dishSuitChangeVersion',0),('cey_bt_table','ch_tableno','dishTableList','dtChTableNo','dishTableVersion',0),('cey_bt_table_area','ch_areano','dishTableAreaList','dtaChAreaNo','dishTableAreaVersion',0),('cey_bt_table_type','ch_typeno','dishTableTypeList','dttChTyperNo','dishTableTypeVersion',0),('cey_bt_unit','ch_unitno','dishUnitList','duChUnitNo','dishUnitVersion',0),('material_bt_dish','int_flowID','dishItemList','intFlowID','dishItemVersion',0),('t_m_curamount','vch_memberno','memberCuramountList','vchMemberno','memberCuramountVersion',0),('t_m_gift','int_flowID','memberGiftList','intFlowid','memberGiftVersion',0),('t_m_member','vch_memberno','memberInfoList','miVchMemberNo','memberInfoVersion',0),('t_m_member_type','ch_typeno','memberTypeList','chTypeno','memberTypeVersion',0);
/*!40000 ALTER TABLE `cey_web_table_version` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cey_web_weixinpay`
--

DROP TABLE IF EXISTS `cey_web_weixinpay`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cey_web_weixinpay` (
  `out_trade_no` varchar(32) NOT NULL,
  `appid` varchar(32) NOT NULL,
  `appkey` varchar(128) NOT NULL,
  `mch_id` varchar(32) NOT NULL,
  UNIQUE KEY `out_trade_no` (`out_trade_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cey_web_weixinpay`
--

LOCK TABLES `cey_web_weixinpay` WRITE;
/*!40000 ALTER TABLE `cey_web_weixinpay` DISABLE KEYS */;
/*!40000 ALTER TABLE `cey_web_weixinpay` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chudaloudan`
--

DROP TABLE IF EXISTS `chudaloudan`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chudaloudan` (
  `id` int(11) NOT NULL,
  `vch_ip` varchar(20) DEFAULT NULL,
  `vch_content` varchar(2048) DEFAULT NULL,
  `vch_danaho` varchar(20) DEFAULT NULL,
  `vch_title` varchar(30) DEFAULT NULL,
  `vch_printflag` varchar(5) DEFAULT NULL,
  `dt_date` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chudaloudan`
--

LOCK TABLES `chudaloudan` WRITE;
/*!40000 ALTER TABLE `chudaloudan` DISABLE KEYS */;
/*!40000 ALTER TABLE `chudaloudan` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cybr_book_detail`
--

DROP TABLE IF EXISTS `cybr_book_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cybr_book_detail` (
  `ch_bookno` varchar(12) NOT NULL,
  `int_id` int(11) NOT NULL,
  `ch_dishno` varchar(7) NOT NULL,
  `vch_spec` varchar(12) DEFAULT NULL,
  `num_num` decimal(14,2) DEFAULT NULL,
  `num_price` decimal(14,2) DEFAULT NULL,
  `vch_print_memo` varchar(1023) DEFAULT NULL,
  `num_price_add` decimal(14,2) DEFAULT NULL,
  `ch_suitflag` varchar(1) DEFAULT NULL,
  `ch_suitno` varchar(7) DEFAULT NULL,
  `int_material_rate` int(11) DEFAULT NULL,
  `ch_fixprice` varchar(1) DEFAULT NULL,
  `ch_specialflag` varchar(1) DEFAULT NULL,
  `ch_eatflag` varchar(1) NOT NULL DEFAULT 'N',
  PRIMARY KEY (`ch_bookno`,`int_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cybr_book_detail`
--

LOCK TABLES `cybr_book_detail` WRITE;
/*!40000 ALTER TABLE `cybr_book_detail` DISABLE KEYS */;
/*!40000 ALTER TABLE `cybr_book_detail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cybr_book_master`
--

DROP TABLE IF EXISTS `cybr_book_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cybr_book_master` (
  `ch_bookno` varchar(12) NOT NULL,
  `ch_type` varchar(1) NOT NULL,
  `dt_come` datetime DEFAULT NULL,
  `int_overtime` int(11) DEFAULT NULL,
  `dt_warn` datetime DEFAULT NULL,
  `int_person` int(11) DEFAULT NULL,
  `vch_tel` varchar(20) DEFAULT NULL,
  `vch_booker` varchar(20) DEFAULT NULL,
  `ch_booker_type` varchar(1) DEFAULT NULL,
  `vch_ID` varchar(20) DEFAULT NULL,
  `vch_tel_second` varchar(20) DEFAULT NULL,
  `vch_address` varchar(50) DEFAULT NULL,
  `vch_memo` varchar(50) DEFAULT NULL,
  `num_deposit` decimal(14,2) DEFAULT NULL,
  `num_table_price` decimal(14,2) DEFAULT NULL,
  `ch_pricekind` varchar(1) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `dt_come_real` datetime DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_sex` varchar(20) DEFAULT NULL,
  `ch_suitno` varchar(7) DEFAULT NULL,
  `vch_markid` varchar(6) DEFAULT NULL,
  `vch_tables` varchar(100) DEFAULT NULL,
  `ch_upflag` varchar(1) DEFAULT 'N',
  `ch_bustype` varchar(2) DEFAULT NULL,
  `suit_num` decimal(14,2) DEFAULT '0.00',
  `vch_liking` varchar(50) DEFAULT NULL,
  `vch_memo1` varchar(50) DEFAULT NULL,
  `vch_memo2` varchar(50) DEFAULT NULL,
  `vch_memo3` varchar(50) DEFAULT NULL,
  `dt_come_lunar` datetime DEFAULT NULL,
  `vch_character` varchar(40) DEFAULT NULL,
  `int_seat` int(11) DEFAULT NULL,
  `ch_mode` varchar(1) DEFAULT '0',
  `vch_fax` varchar(40) DEFAULT NULL,
  `ch_interno` varchar(15) DEFAULT NULL,
  PRIMARY KEY (`ch_bookno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cybr_book_master`
--

LOCK TABLES `cybr_book_master` WRITE;
/*!40000 ALTER TABLE `cybr_book_master` DISABLE KEYS */;
/*!40000 ALTER TABLE `cybr_book_master` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `cybr_book_table`
--

DROP TABLE IF EXISTS `cybr_book_table`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cybr_book_table` (
  `ch_bookno` varchar(12) NOT NULL,
  `ch_tableno` varchar(4) NOT NULL,
  `ch_open_flag` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_bookno`,`ch_tableno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cybr_book_table`
--

LOCK TABLES `cybr_book_table` WRITE;
/*!40000 ALTER TABLE `cybr_book_table` DISABLE KEYS */;
/*!40000 ALTER TABLE `cybr_book_table` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_cp_dish`
--

DROP TABLE IF EXISTS `del_cey_bt_cp_dish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_cp_dish` (
  `ch_dishno` varchar(7) NOT NULL,
  `ch_no` varchar(4) NOT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_dishno`,`ch_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_cp_dish`
--

LOCK TABLES `del_cey_bt_cp_dish` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_cp_dish` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_cp_dish` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_cp_memo`
--

DROP TABLE IF EXISTS `del_cey_bt_cp_memo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_cp_memo` (
  `ch_no` varchar(4) NOT NULL,
  `vch_cp_memo` varchar(40) NOT NULL,
  `ch_typeno` varchar(2) NOT NULL,
  `num_add_price` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_cp_memo`
--

LOCK TABLES `del_cey_bt_cp_memo` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_cp_memo` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_cp_memo` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_cp_type`
--

DROP TABLE IF EXISTS `del_cey_bt_cp_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_cp_type` (
  `ch_typeno` varchar(2) NOT NULL,
  `vch_typename` varchar(20) NOT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_typeno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_cp_type`
--

LOCK TABLES `del_cey_bt_cp_type` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_cp_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_cp_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_dish`
--

DROP TABLE IF EXISTS `del_cey_bt_dish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_dish` (
  `ch_dishno` varchar(7) NOT NULL,
  `vch_dishname` varchar(60) NOT NULL,
  `vch_spell` varchar(40) DEFAULT NULL,
  `vch_english` varchar(40) DEFAULT NULL,
  `ch_dish_typeno` varchar(2) DEFAULT NULL,
  `ch_unitno` varchar(2) DEFAULT NULL,
  `num_price` decimal(12,2) DEFAULT '0.00',
  `ch_suitflag` varchar(1) DEFAULT 'N',
  `ch_discount` varchar(1) DEFAULT 'N',
  `ch_curprice` varchar(1) DEFAULT 'N',
  `ch_tempflag` varchar(1) DEFAULT 'N',
  `ch_stopflag` varchar(1) DEFAULT NULL,
  `ch_printflag` varchar(1) DEFAULT NULL,
  `vch_printer` varchar(50) DEFAULT NULL,
  `ch_sub_typeno` varchar(4) DEFAULT NULL,
  `ch_materialno` varchar(7) DEFAULT NULL,
  `ch_serviceflag` varchar(1) DEFAULT NULL,
  `ch_lowflag` varchar(1) DEFAULT NULL,
  `num_m_price` decimal(12,2) DEFAULT '0.00',
  `ch_deductflag` varchar(1) DEFAULT NULL,
  `ch_deducttype` varchar(1) DEFAULT NULL,
  `int_deduct` int(11) DEFAULT NULL,
  `num_deduct` decimal(12,2) DEFAULT NULL,
  `ch_saleflag` varchar(1) DEFAULT NULL,
  `ch_saletype` varchar(1) DEFAULT NULL,
  `int_sale_deduct` int(11) DEFAULT NULL,
  `num_sale` decimal(12,2) DEFAULT NULL,
  `item_flag` varchar(1) DEFAULT NULL,
  `vch_pur_unitno` varchar(2) DEFAULT NULL,
  `int_unit_rate` int(11) DEFAULT NULL,
  `ch_off` varchar(1) DEFAULT 'N',
  `vch_subno` varchar(40) DEFAULT NULL,
  `num_sale_price` decimal(20,2) DEFAULT '0.00',
  `ch_outflag` varchar(1) DEFAULT NULL,
  `vch_outprint` varchar(50) DEFAULT NULL,
  `int_rate` decimal(12,2) DEFAULT NULL,
  `ch_weight` varchar(1) DEFAULT NULL,
  `vch_barcode` varchar(64) DEFAULT NULL,
  `ch_kitchen_print_flag` varchar(1) DEFAULT NULL,
  `vch_kitchen_print_id1` varchar(2) DEFAULT NULL,
  `vch_kitchen_print_id2` varchar(2) DEFAULT NULL,
  `vch_kitchen_print_id3` varchar(2) DEFAULT NULL,
  `ch_kitchen_out_flag` varchar(1) DEFAULT NULL,
  `vch_kitchen_out_id1` varchar(2) DEFAULT NULL,
  `vch_kitchen_out_id2` varchar(2) DEFAULT NULL,
  `vch_kitchen_out_id3` varchar(2) DEFAULT NULL,
  `ch_groupno` varchar(7) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `num_item_warn` decimal(12,2) DEFAULT '0.00',
  `ch_guqing_flag` varchar(1) DEFAULT NULL,
  `num_guqing_num` decimal(12,2) DEFAULT '0.00',
  `ch_material_flag` varchar(1) DEFAULT NULL,
  `ch_special_type` varchar(1) DEFAULT '0',
  `ch_special_type2` varchar(1) DEFAULT '0',
  `ch_special_type3` varchar(1) DEFAULT '0',
  `ch_special_type4` varchar(1) DEFAULT '0',
  `num_special_price` decimal(12,3) DEFAULT '0.000',
  `vch_dishname_2` varchar(60) DEFAULT NULL,
  `vch_color` varchar(10) DEFAULT NULL,
  `ch_count` varchar(1) DEFAULT 'N',
  `num_count_weight` decimal(12,3) DEFAULT '0.000',
  `ch_qr_code_online` varchar(1) DEFAULT '',
  `int_rate_2` decimal(12,2) DEFAULT '0.00',
  `int_rate_3` decimal(12,2) DEFAULT '0.00',
  `ch_addon_flag` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_dishno`),
  KEY `index_name` (`ch_groupno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_dish`
--

LOCK TABLES `del_cey_bt_dish` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_dish` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_dish` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_dish_subtype`
--

DROP TABLE IF EXISTS `del_cey_bt_dish_subtype`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_dish_subtype` (
  `ch_sub_typeno` varchar(4) NOT NULL,
  `vch_sub_typename` varchar(20) NOT NULL,
  `ch_dish_typeno` varchar(2) NOT NULL,
  `ch_labelprint_flag` varchar(1) DEFAULT 'N',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `vch_color` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`ch_sub_typeno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_dish_subtype`
--

LOCK TABLES `del_cey_bt_dish_subtype` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_dish_subtype` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_dish_subtype` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_dish_suit`
--

DROP TABLE IF EXISTS `del_cey_bt_dish_suit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_dish_suit` (
  `int_flowid` int(11) NOT NULL AUTO_INCREMENT,
  `ch_suitno` varchar(7) NOT NULL,
  `ch_dishno` varchar(7) DEFAULT NULL,
  `num_num` decimal(12,2) DEFAULT '1.00',
  `ch_dishno_chang` varchar(7) DEFAULT NULL,
  `num_num_change` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`int_flowid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_dish_suit`
--

LOCK TABLES `del_cey_bt_dish_suit` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_dish_suit` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_dish_suit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_dish_type`
--

DROP TABLE IF EXISTS `del_cey_bt_dish_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_dish_type` (
  `ch_dish_typeno` varchar(2) NOT NULL,
  `vch_dish_typename` varchar(20) NOT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `vch_color` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`ch_dish_typeno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_dish_type`
--

LOCK TABLES `del_cey_bt_dish_type` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_dish_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_dish_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_dish_warn`
--

DROP TABLE IF EXISTS `del_cey_bt_dish_warn`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_dish_warn` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `dt_date` datetime NOT NULL,
  `ch_dishno` varchar(7) DEFAULT NULL,
  `num_num` decimal(12,2) DEFAULT NULL,
  `num_sale` decimal(12,2) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_operID2` varchar(6) DEFAULT NULL,
  `dt_operdate2` datetime DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_dish_warn`
--

LOCK TABLES `del_cey_bt_dish_warn` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_dish_warn` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_dish_warn` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_paymode`
--

DROP TABLE IF EXISTS `del_cey_bt_paymode`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_paymode` (
  `ch_paymodeno` varchar(2) NOT NULL,
  `vch_paymodename` varchar(60) NOT NULL,
  `ch_IDflag` varchar(1) DEFAULT NULL,
  `ch_faceflag` varchar(1) DEFAULT NULL,
  `ch_changeflag` varchar(1) DEFAULT NULL,
  `ch_incomeflag` varchar(1) DEFAULT NULL,
  `ch_key` varchar(1) DEFAULT NULL,
  `vch_arg1` varchar(128) DEFAULT NULL,
  `vch_arg2` varchar(128) DEFAULT NULL,
  `vch_arg3` text,
  `vch_arg4` varchar(128) DEFAULT NULL,
  `vch_arg5` varchar(128) DEFAULT NULL,
  `vch_arg6` varchar(128) DEFAULT NULL,
  `vch_arg7` varchar(128) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `vch_english` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`ch_paymodeno`),
  UNIQUE KEY `vch_english` (`vch_english`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_paymode`
--

LOCK TABLES `del_cey_bt_paymode` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_paymode` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_paymode` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_suit_change`
--

DROP TABLE IF EXISTS `del_cey_bt_suit_change`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_suit_change` (
  `int_flowID` int(11) NOT NULL,
  `ch_dishno` varchar(7) NOT NULL,
  `num_num_change` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_dishno`,`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_suit_change`
--

LOCK TABLES `del_cey_bt_suit_change` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_suit_change` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_suit_change` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_table`
--

DROP TABLE IF EXISTS `del_cey_bt_table`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_table` (
  `ch_tableno` varchar(4) NOT NULL,
  `vch_tablename` varchar(12) NOT NULL,
  `ch_typeno` varchar(2) DEFAULT NULL,
  `ch_billno` varchar(15) DEFAULT NULL,
  `ch_lockflag` varchar(1) DEFAULT NULL,
  `int_person` int(11) DEFAULT NULL,
  `vch_memo` varchar(100) DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT NULL,
  `dt_datetime` datetime DEFAULT NULL,
  `ch_areaNo` varchar(2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `ch_billno_1` varchar(15) DEFAULT NULL,
  `ch_billno_2` varchar(15) DEFAULT NULL,
  `ch_billno_3` varchar(15) DEFAULT NULL,
  `ch_billno_4` varchar(15) DEFAULT NULL,
  `ch_billno_5` varchar(15) DEFAULT NULL,
  `ch_billno_6` varchar(15) DEFAULT NULL,
  `ch_billno_7` varchar(15) DEFAULT NULL,
  `ch_billno_8` varchar(15) DEFAULT NULL,
  `ch_lockflag_1` varchar(1) DEFAULT NULL,
  `ch_lockflag_2` varchar(1) DEFAULT NULL,
  `ch_lockflag_3` varchar(1) DEFAULT NULL,
  `ch_lockflag_4` varchar(1) DEFAULT NULL,
  `ch_lockflag_5` varchar(1) DEFAULT NULL,
  `ch_lockflag_6` varchar(1) DEFAULT NULL,
  `ch_lockflag_7` varchar(1) DEFAULT NULL,
  `ch_lockflag_8` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_tableno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_table`
--

LOCK TABLES `del_cey_bt_table` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_table` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_table` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_table_area`
--

DROP TABLE IF EXISTS `del_cey_bt_table_area`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_table_area` (
  `ch_areano` varchar(2) NOT NULL,
  `vch_areaname` varchar(12) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_areano`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_table_area`
--

LOCK TABLES `del_cey_bt_table_area` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_table_area` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_table_area` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_table_type`
--

DROP TABLE IF EXISTS `del_cey_bt_table_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_table_type` (
  `ch_typeno` varchar(2) NOT NULL,
  `vch_typename` varchar(12) NOT NULL,
  `ch_pricekind` varchar(1) DEFAULT NULL,
  `ch_service_mode` varchar(1) DEFAULT NULL,
  `ch_serviceflag` varchar(1) DEFAULT NULL,
  `int_start_minute` int(11) DEFAULT NULL,
  `num_start_amount` decimal(12,2) DEFAULT NULL,
  `int_unit_minute` int(11) DEFAULT NULL,
  `num_unit_amount` decimal(12,2) DEFAULT NULL,
  `int_over` int(11) DEFAULT NULL,
  `num_ration` decimal(12,2) DEFAULT NULL,
  `int_rate` int(11) DEFAULT NULL,
  `num_max_service` decimal(12,2) DEFAULT NULL,
  `ch_low_mode` varchar(1) DEFAULT NULL,
  `num_lowcost` decimal(12,2) DEFAULT NULL,
  `int_begin_time` int(11) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_typeno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_table_type`
--

LOCK TABLES `del_cey_bt_table_type` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_table_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_table_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_cey_bt_unit`
--

DROP TABLE IF EXISTS `del_cey_bt_unit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_cey_bt_unit` (
  `ch_unitno` varchar(2) NOT NULL,
  `vch_unitname` varchar(10) NOT NULL,
  `ch_numtype` varchar(1) DEFAULT '0',
  `num_default` decimal(12,2) DEFAULT '1.00',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_unitno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_cey_bt_unit`
--

LOCK TABLES `del_cey_bt_unit` WRITE;
/*!40000 ALTER TABLE `del_cey_bt_unit` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_cey_bt_unit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_material_bt_dish`
--

DROP TABLE IF EXISTS `del_material_bt_dish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_material_bt_dish` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT COMMENT '流水号',
  `ch_dishno` varchar(7) NOT NULL COMMENT '菜品编码',
  `ch_itemno` varchar(7) NOT NULL COMMENT '原料编码',
  `num_num` decimal(12,2) DEFAULT '0.00' COMMENT '原料数量',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL COMMENT '是否上传',
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_material_bt_dish`
--

LOCK TABLES `del_material_bt_dish` WRITE;
/*!40000 ALTER TABLE `del_material_bt_dish` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_material_bt_dish` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_t_m_curamount`
--

DROP TABLE IF EXISTS `del_t_m_curamount`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_t_m_curamount` (
  `vch_memberno` varchar(20) NOT NULL,
  `num_amount` decimal(12,2) DEFAULT NULL,
  `num_hangamount` decimal(12,2) DEFAULT NULL,
  `num_point` decimal(12,2) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_branchno` varchar(4) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`vch_memberno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_t_m_curamount`
--

LOCK TABLES `del_t_m_curamount` WRITE;
/*!40000 ALTER TABLE `del_t_m_curamount` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_t_m_curamount` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_t_m_gift`
--

DROP TABLE IF EXISTS `del_t_m_gift`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_t_m_gift` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `ch_giftno` varchar(4) NOT NULL,
  `vch_giftname` varchar(30) NOT NULL,
  `num_point` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_t_m_gift`
--

LOCK TABLES `del_t_m_gift` WRITE;
/*!40000 ALTER TABLE `del_t_m_gift` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_t_m_gift` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_t_m_member`
--

DROP TABLE IF EXISTS `del_t_m_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_t_m_member` (
  `vch_memberno` varchar(20) NOT NULL,
  `vch_member` varchar(40) NOT NULL,
  `ch_typeno` varchar(2) DEFAULT NULL,
  `vch_id` varchar(20) DEFAULT NULL,
  `dt_birthday` datetime DEFAULT NULL,
  `vch_tel` varchar(20) DEFAULT NULL,
  `vch_handtel` varchar(20) DEFAULT NULL,
  `vch_address` varchar(50) DEFAULT NULL,
  `dt_limit` datetime DEFAULT NULL,
  `ch_state` varchar(1) DEFAULT '1',
  `num_limit` decimal(12,2) DEFAULT NULL,
  `ch_cardflag` varchar(1) DEFAULT 'N',
  `vch_cardno` varchar(20) DEFAULT NULL,
  `vch_password` varchar(20) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `int_basedata` int(11) DEFAULT '0',
  `dt_sendtime` datetime DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`vch_memberno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_t_m_member`
--

LOCK TABLES `del_t_m_member` WRITE;
/*!40000 ALTER TABLE `del_t_m_member` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_t_m_member` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `del_t_m_member_type`
--

DROP TABLE IF EXISTS `del_t_m_member_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `del_t_m_member_type` (
  `ch_typeno` varchar(2) NOT NULL,
  `vch_typename` varchar(16) NOT NULL,
  `ch_attribute` varchar(1) NOT NULL,
  `ch_favourable` varchar(1) DEFAULT NULL,
  `ch_pricekind` varchar(1) DEFAULT NULL,
  `int_discount` int(11) DEFAULT NULL,
  `ch_projectno` varchar(3) DEFAULT NULL,
  `ch_calc` varchar(1) DEFAULT NULL,
  `ch_calc_type` varchar(1) DEFAULT NULL,
  `int_calc_unit` int(11) DEFAULT NULL,
  `ch_area_flag` varchar(1) DEFAULT NULL,
  `int_backcash_rate` int(11) DEFAULT NULL,
  `int_card_baseamount` int(11) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_typeno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `del_t_m_member_type`
--

LOCK TABLES `del_t_m_member_type` WRITE;
/*!40000 ALTER TABLE `del_t_m_member_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `del_t_m_member_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `interface_malayisa_hourly_pos`
--

DROP TABLE IF EXISTS `interface_malayisa_hourly_pos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `interface_malayisa_hourly_pos` (
  `int_batchid` int(11) NOT NULL,
  `dt_operdate` date NOT NULL,
  `ch_state` varchar(1) NOT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`int_batchid`),
  UNIQUE KEY `dt_operdate` (`dt_operdate`),
  KEY `idx_ch_state` (`ch_state`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `interface_malayisa_hourly_pos`
--

LOCK TABLES `interface_malayisa_hourly_pos` WRITE;
/*!40000 ALTER TABLE `interface_malayisa_hourly_pos` DISABLE KEYS */;
/*!40000 ALTER TABLE `interface_malayisa_hourly_pos` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `material_bt_dish`
--

DROP TABLE IF EXISTS `material_bt_dish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `material_bt_dish` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT COMMENT '流水号',
  `ch_dishno` varchar(7) NOT NULL COMMENT '菜品编码',
  `ch_itemno` varchar(7) NOT NULL COMMENT '原料编码',
  `num_num` decimal(12,3) DEFAULT '0.000',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL COMMENT '是否上传',
  PRIMARY KEY (`int_flowID`),
  KEY `index_name` (`ch_dishno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `material_bt_dish`
--

LOCK TABLES `material_bt_dish` WRITE;
/*!40000 ALTER TABLE `material_bt_dish` DISABLE KEYS */;
/*!40000 ALTER TABLE `material_bt_dish` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_material_bt_dish_insert AFTER INSERT ON material_bt_dish FOR EACH ROW  Begin  delete from del_material_bt_dish where int_flowID=NEW.int_flowID; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_material_bt_dish_update BEFORE UPDATE ON material_bt_dish FOR EACH ROW  Begin   IF(NEW.int_flowID = OLD.int_flowID and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_material_bt_dish_delete BEFORE DELETE ON material_bt_dish FOR EACH ROW  Begin  replace into del_material_bt_dish select * from material_bt_dish where int_flowID=OLD.int_flowID; update del_material_bt_dish set ch_yun_already_keep = 'N' where int_flowID=OLD.int_flowID; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `material_loss_apply`
--

DROP TABLE IF EXISTS `material_loss_apply`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `material_loss_apply` (
  `BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',
  `ORG_CODE` varchar(64) DEFAULT NULL COMMENT '受理总店',
  `ORG_NAME` varchar(64) DEFAULT NULL,
  `PEOPLE` varchar(32) DEFAULT NULL COMMENT '审核人',
  `REMARK` varchar(256) DEFAULT NULL COMMENT '备注',
  `CH_STATE` varchar(1) DEFAULT NULL COMMENT '审核状态',
  `DATE` datetime DEFAULT NULL COMMENT '表单时间',
  `TITLE` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`BILL_NO`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='报损申请表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `material_loss_apply`
--

LOCK TABLES `material_loss_apply` WRITE;
/*!40000 ALTER TABLE `material_loss_apply` DISABLE KEYS */;
/*!40000 ALTER TABLE `material_loss_apply` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `material_loss_apply_detail`
--

DROP TABLE IF EXISTS `material_loss_apply_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `material_loss_apply_detail` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',
  `MATERIAL_CODE` varchar(32) DEFAULT NULL COMMENT '原料编码',
  `MATERIAL_NAME` varchar(32) DEFAULT NULL COMMENT '原料名称',
  `FORMAT` varchar(64) DEFAULT NULL COMMENT '规格',
  `UNIT_CODE` varchar(16) DEFAULT NULL COMMENT '入库单位',
  `UNIT_NAME` varchar(32) DEFAULT NULL COMMENT '单位名称',
  `APPLY_NUM` decimal(16,2) DEFAULT NULL COMMENT '出库',
  `APPLY_PRICE` decimal(16,2) DEFAULT NULL COMMENT '入库价格',
  `DATE` datetime DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='报损申请明细';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `material_loss_apply_detail`
--

LOCK TABLES `material_loss_apply_detail` WRITE;
/*!40000 ALTER TABLE `material_loss_apply_detail` DISABLE KEYS */;
/*!40000 ALTER TABLE `material_loss_apply_detail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `material_purchase_apply`
--

DROP TABLE IF EXISTS `material_purchase_apply`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `material_purchase_apply` (
  `BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',
  `APPLY_PEOPLE` varchar(32) DEFAULT NULL COMMENT '申请人',
  `APPLY_TIME` datetime DEFAULT NULL COMMENT '申请时间',
  `CH_STATE` varchar(1) DEFAULT NULL COMMENT '审核状态',
  `REMARK` varchar(256) DEFAULT NULL COMMENT '备注',
  `DATE` datetime DEFAULT NULL COMMENT '表单时间',
  `TITLE` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`BILL_NO`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='采购申请表';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `material_purchase_apply`
--

LOCK TABLES `material_purchase_apply` WRITE;
/*!40000 ALTER TABLE `material_purchase_apply` DISABLE KEYS */;
/*!40000 ALTER TABLE `material_purchase_apply` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `material_purchase_apply_detail`
--

DROP TABLE IF EXISTS `material_purchase_apply_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `material_purchase_apply_detail` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',
  `MATERIAL_CODE` varchar(32) DEFAULT NULL COMMENT '原料编码',
  `MATERIAL_NAME` varchar(32) DEFAULT NULL COMMENT '原料名称',
  `FORMAT` varchar(64) DEFAULT NULL COMMENT '规格',
  `UNIT_CODE` varchar(16) DEFAULT NULL COMMENT '入库单位',
  `UNIT_NAME` varchar(32) DEFAULT NULL COMMENT '单位名称',
  `APPLY_NUM` decimal(16,2) DEFAULT NULL COMMENT '申请数量',
  `DATE` datetime DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='原料采购申请明细';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `material_purchase_apply_detail`
--

LOCK TABLES `material_purchase_apply_detail` WRITE;
/*!40000 ALTER TABLE `material_purchase_apply_detail` DISABLE KEYS */;
/*!40000 ALTER TABLE `material_purchase_apply_detail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `material_purchase_in_storage`
--

DROP TABLE IF EXISTS `material_purchase_in_storage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `material_purchase_in_storage` (
  `BILL_NO` varchar(64) NOT NULL COMMENT '编码',
  `ORG_CODE` varchar(64) DEFAULT NULL COMMENT '入库总店',
  `ORG_NAME` varchar(64) DEFAULT NULL COMMENT '入库总店',
  `PEOPLE` varchar(32) DEFAULT NULL COMMENT '入库人',
  `REMARK` varchar(128) DEFAULT NULL COMMENT '备注',
  `CH_STATE` varchar(1) DEFAULT NULL COMMENT '入库状态',
  `DATE` datetime DEFAULT NULL,
  `TITLE` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`BILL_NO`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='原料采购入库单';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `material_purchase_in_storage`
--

LOCK TABLES `material_purchase_in_storage` WRITE;
/*!40000 ALTER TABLE `material_purchase_in_storage` DISABLE KEYS */;
/*!40000 ALTER TABLE `material_purchase_in_storage` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `material_purchase_in_storage_detail`
--

DROP TABLE IF EXISTS `material_purchase_in_storage_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `material_purchase_in_storage_detail` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `BILL_NO` varchar(64) NOT NULL COMMENT '单据编号',
  `MATERIAL_CODE` varchar(32) DEFAULT NULL COMMENT '原料编码',
  `MATERIAL_NAME` varchar(32) DEFAULT NULL COMMENT '原料名称',
  `FORMAT` varchar(64) DEFAULT NULL COMMENT '规格',
  `UNIT_CODE` varchar(16) DEFAULT NULL COMMENT '入库单位',
  `UNIT_NAME` varchar(32) DEFAULT NULL COMMENT '单位名称',
  `APPLY_NUM` decimal(16,2) DEFAULT NULL COMMENT '入库数量',
  `APPLY_PRICE` decimal(16,2) DEFAULT NULL COMMENT '入库价格',
  `DATE` datetime DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='原料采购入库单明细';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `material_purchase_in_storage_detail`
--

LOCK TABLES `material_purchase_in_storage_detail` WRITE;
/*!40000 ALTER TABLE `material_purchase_in_storage_detail` DISABLE KEYS */;
/*!40000 ALTER TABLE `material_purchase_in_storage_detail` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `material_u_orderdish`
--

DROP TABLE IF EXISTS `material_u_orderdish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `material_u_orderdish` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT COMMENT '流水号',
  `int_order_flowID` int(11) NOT NULL COMMENT '主菜流水',
  `ch_dishno` varchar(7) NOT NULL COMMENT '原料',
  `num_num` decimal(12,3) DEFAULT '0.000' COMMENT '原料点菜数量',
  `num_back` decimal(12,3) DEFAULT '0.000' COMMENT '原料退菜数量',
  `num_rate` decimal(12,3) DEFAULT '0.000' COMMENT '原料主料比率2',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL COMMENT '是否上传',
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `material_u_orderdish`
--

LOCK TABLES `material_u_orderdish` WRITE;
/*!40000 ALTER TABLE `material_u_orderdish` DISABLE KEYS */;
/*!40000 ALTER TABLE `material_u_orderdish` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER tr_material_u_orderdish_insert AFTER INSERT ON material_u_orderdish FOR EACH ROW 
       BEGIN
       DECLARE flowId integer;
       DECLARE dishno varchar(7);
       DECLARE numnum numeric(10, 3);
       DECLARE materialno varchar(7);
       DECLARE intflowId integer;
       DECLARE initqty numeric(16,4);
       DECLARE initprice numeric(16,4);
       DECLARE remainqty numeric(16,4);
       DECLARE remainprice numeric(16,4);
       DECLARE itemflag char(1);
       set flowId = NEW.int_flowId;
       set dishno = NEW.ch_dishno;
       set numnum = NEW.num_num - NEW.num_back;
       set itemflag = 'N';
       set itemflag = (select ifnull(item_flag,'N') from cey_bt_dish where ch_dishno = dishno);
       set materialno = dishno;
        IF (materialno<>'' and itemflag = 'Y') THEN
       set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow where ch_materialno = materialno);
       IF (intflowId = 0) THEN
          set initqty = 0;
       ELSE
          set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);
       END IF;
      IF (intflowId = 0) THEN
          set initprice = 0;
      ELSE
          set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);
      END IF;
        set remainqty  = initqty - numnum ;
     IF (remainqty = 0) THEN
         set remainprice = 0;
     ELSE
          set remainprice = initprice;
       END IF;
           insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
              num_num, sheet_price, cost_price, remain_qty, remain_price, ch_sheetno )
        values (materialno,now(), 'YL', if(numnum < 0, -1, 1), abs(numnum), 0, remainprice, remainqty, remainprice, LEFT(flowId,20));
      IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN
            update cey_st_material_num set num_num = remainqty, cost_price = remainprice
          where ch_materialno = materialno;
       ELSE
       insert into cey_st_material_num (ch_materialno, num_num, cost_price)
      values (materialno, remainqty, remainprice);
      END IF;
       END IF;
       END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER tr_material_u_orderdish_update AFTER UPDATE ON material_u_orderdish FOR EACH ROW 
      BEGIN
       DECLARE m_done int default 0;
     DECLARE flowId integer;
     DECLARE dishno varchar(7);
     DECLARE numnum numeric(10, 3);
  DECLARE materialno varchar(7);
    DECLARE intflowId integer;
       DECLARE initqty numeric(16,4);
      DECLARE initprice numeric(16,4);
     DECLARE remainqty numeric(16,4);
     DECLARE remainprice numeric(16,4);
       DECLARE itemflag char(1);
    IF(NEW.int_flowId = OLD.int_flowId and (NEW.num_back <> OLD.num_back or NEW.num_num <> OLD.num_num ) ) THEN
        set flowId = NEW.int_flowId;
      set dishno = NEW.ch_dishno;
      set numnum = OLD.num_num - OLD.num_back - (NEW.num_num - NEW.num_back);
     set itemflag = 'N';
        set itemflag = (select ifnull(item_flag,'N') from cey_bt_dish where ch_dishno = dishno);
         set materialno = dishno;
        IF (materialno<>'' and itemflag = 'Y')  THEN
    set intflowId = (select ifnull(max(int_flowId),0) from cey_st_material_flow where ch_materialno = materialno);
       IF (intflowId = 0) THEN
             set initqty = 0;
    ELSE
         set initqty = (select remain_qty from cey_st_material_flow where int_flowId = intflowId);
      END IF;
        IF (intflowId = 0) THEN
          set initprice = 0;
    ELSE
      set initprice = (select remain_price from cey_st_material_flow where int_flowId = intflowId);
       END IF;
       set remainqty = initqty + numnum ;
        IF (remainqty = 0)  THEN
       set remainprice = 0;
      ELSE
       set remainprice = initprice;
        END IF;
           insert cey_st_material_flow (ch_materialno, dt_operdate, sheettype, int_flag,
                     num_num, sheet_price, cost_price, remain_qty, remain_price, ch_sheetno )
          values (materialno,now(), 'YL', if(numnum < 0, -1, 1), abs(numnum), 0, remainprice, remainqty, remainprice, left(flowId,20));
     IF exists (select 1 from cey_st_material_num where ch_materialno = materialno) THEN
       update cey_st_material_num set num_num = remainqty, cost_price = remainprice
      where ch_materialno = materialno;
          ELSE
           insert into cey_st_material_num (ch_materialno, num_num, cost_price)
              values (materialno, remainqty, remainprice);
      END IF;
     END IF;
          END IF;
   END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `mess_t_mess_flow`
--

DROP TABLE IF EXISTS `mess_t_mess_flow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_mess_flow` (
  `flow_id` decimal(20,0) NOT NULL,
  `cust_no` varchar(20) DEFAULT NULL,
  `group_no` varchar(4) DEFAULT NULL,
  `mobile` varchar(20) DEFAULT NULL,
  `oper_id` varchar(6) DEFAULT NULL,
  `oper_date` datetime DEFAULT NULL,
  `send_date` datetime DEFAULT NULL,
  `mess` varchar(200) DEFAULT NULL,
  `region` varchar(1) DEFAULT NULL,
  `mess_id` decimal(20,0) DEFAULT NULL,
  `memo` varchar(100) DEFAULT NULL,
  `cust_name` varchar(20) DEFAULT NULL,
  `plan_id` decimal(20,0) DEFAULT NULL,
  `inout_type` varchar(1) DEFAULT NULL,
  `branch_no` varchar(10) DEFAULT NULL,
  `error` varchar(200) DEFAULT NULL,
  PRIMARY KEY (`flow_id`),
  KEY `i_bd_supcust_item_1` (`group_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_mess_flow`
--

LOCK TABLES `mess_t_mess_flow` WRITE;
/*!40000 ALTER TABLE `mess_t_mess_flow` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_mess_flow` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_set_plan`
--

DROP TABLE IF EXISTS `mess_t_set_plan`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_set_plan` (
  `flow_id` int(11) NOT NULL AUTO_INCREMENT,
  `plan_type` varchar(1) DEFAULT NULL,
  `group_no` varchar(11) DEFAULT NULL,
  `group_name` varchar(30) DEFAULT NULL,
  `region` varchar(1) DEFAULT NULL,
  `date_type` varchar(2) DEFAULT NULL,
  `send_date` varchar(10) DEFAULT NULL,
  `send_time` datetime DEFAULT NULL,
  `plan_desc` varchar(20) DEFAULT NULL,
  `send_mess` varchar(140) DEFAULT NULL,
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_set_plan`
--

LOCK TABLES `mess_t_set_plan` WRITE;
/*!40000 ALTER TABLE `mess_t_set_plan` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_set_plan` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_sys_system`
--

DROP TABLE IF EXISTS `mess_t_sys_system`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_sys_system` (
  `sys_var_id` varchar(20) NOT NULL,
  `sys_var_name` varchar(40) DEFAULT NULL,
  `sys_var_value` varchar(140) DEFAULT NULL,
  PRIMARY KEY (`sys_var_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_sys_system`
--

LOCK TABLES `mess_t_sys_system` WRITE;
/*!40000 ALTER TABLE `mess_t_sys_system` DISABLE KEYS */;
INSERT INTO `mess_t_sys_system` VALUES ('auto_time','系统变量auto_time','5'),('daysum','系统变量daysum','0/23/59/'),('recommend','系统变量recommend','1');
/*!40000 ALTER TABLE `mess_t_sys_system` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_accnum`
--

DROP TABLE IF EXISTS `mess_t_vip_accnum`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_accnum` (
  `flow_id` decimal(20,0) NOT NULL,
  `card_id` varchar(20) NOT NULL,
  `acc_num` decimal(18,4) DEFAULT NULL,
  `send_flag` varchar(1) DEFAULT '0',
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_accnum`
--

LOCK TABLES `mess_t_vip_accnum` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_accnum` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_accnum` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_birthday`
--

DROP TABLE IF EXISTS `mess_t_vip_birthday`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_birthday` (
  `flow_id` decimal(20,0) NOT NULL,
  `card_id` varchar(20) NOT NULL,
  `send_year` varchar(4) NOT NULL,
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_birthday`
--

LOCK TABLES `mess_t_vip_birthday` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_birthday` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_birthday` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_boss`
--

DROP TABLE IF EXISTS `mess_t_vip_boss`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_boss` (
  `flow_id` decimal(20,0) NOT NULL,
  `oper_date` varchar(10) NOT NULL,
  `ope_date` datetime NOT NULL,
  `card_id` varchar(20) NOT NULL,
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_boss`
--

LOCK TABLES `mess_t_vip_boss` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_boss` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_boss` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_card_little`
--

DROP TABLE IF EXISTS `mess_t_vip_card_little`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_card_little` (
  `flow_id` decimal(20,0) NOT NULL,
  `ope_date` datetime NOT NULL,
  `card_id` varchar(20) NOT NULL,
  `card_little` varchar(10) NOT NULL,
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_card_little`
--

LOCK TABLES `mess_t_vip_card_little` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_card_little` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_card_little` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_flow`
--

DROP TABLE IF EXISTS `mess_t_vip_flow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_flow` (
  `flow_id` decimal(20,0) NOT NULL,
  `card_id` varchar(20) NOT NULL,
  `acc_num` decimal(18,4) DEFAULT NULL,
  `flow_amt` decimal(18,4) DEFAULT NULL,
  `oper_type` varchar(1) DEFAULT NULL,
  `send_flag` varchar(1) DEFAULT '0',
  `flow_no` varchar(20) DEFAULT NULL,
  `item_no` varchar(20) DEFAULT NULL,
  `oper_date` datetime DEFAULT NULL,
  `send_flag1` varchar(1) DEFAULT '0',
  `send_time` varchar(2) DEFAULT NULL,
  `consumer_flag` varchar(1) DEFAULT '0',
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_flow`
--

LOCK TABLES `mess_t_vip_flow` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_flow` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_flow` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_giv`
--

DROP TABLE IF EXISTS `mess_t_vip_giv`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_giv` (
  `flow_id` decimal(20,0) NOT NULL,
  `card_id` varchar(20) NOT NULL,
  `father_card` varchar(20) NOT NULL,
  `giv_acc_num` decimal(18,4) DEFAULT NULL,
  `oper_type` varchar(1) DEFAULT NULL,
  `change_amt` decimal(18,4) DEFAULT '0.0000',
  `send_flag` varchar(1) DEFAULT '0',
  `oper_date` datetime DEFAULT NULL,
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_giv`
--

LOCK TABLES `mess_t_vip_giv` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_giv` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_giv` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_info`
--

DROP TABLE IF EXISTS `mess_t_vip_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_info` (
  `flow_id` decimal(20,0) NOT NULL,
  `card_id` varchar(20) NOT NULL,
  `father_card` varchar(20) DEFAULT NULL,
  `change_amt` decimal(18,4) DEFAULT '0.0000',
  `send_flag` varchar(1) DEFAULT '0',
  `oper_date` datetime DEFAULT NULL,
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_info`
--

LOCK TABLES `mess_t_vip_info` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_info` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_message`
--

DROP TABLE IF EXISTS `mess_t_vip_message`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_message` (
  `flow_id` decimal(20,0) NOT NULL,
  `mess_type` varchar(10) NOT NULL,
  `send_flag` varchar(1) NOT NULL DEFAULT '0',
  `oper_date` datetime DEFAULT NULL,
  `branch_no` varchar(10) NOT NULL,
  `card_id` varchar(20) NOT NULL,
  `card_type` varchar(2) DEFAULT NULL,
  `to_card_type` varchar(2) DEFAULT NULL,
  `father_card` varchar(20) DEFAULT NULL,
  `giv_acc_num` decimal(18,4) DEFAULT NULL,
  `oper_type` varchar(1) DEFAULT NULL,
  `change_amt` decimal(18,4) DEFAULT '0.0000',
  `savelist_amt` decimal(18,4) DEFAULT NULL,
  `flow_no` varchar(20) DEFAULT NULL,
  `item_no` varchar(20) DEFAULT NULL,
  `acc_num` decimal(18,4) DEFAULT NULL,
  `flow_amt` decimal(18,4) DEFAULT NULL,
  `send_year` varchar(4) DEFAULT NULL,
  `send_date` varchar(10) DEFAULT NULL,
  `card_little` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_message`
--

LOCK TABLES `mess_t_vip_message` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_message` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_message` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_savelist`
--

DROP TABLE IF EXISTS `mess_t_vip_savelist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_savelist` (
  `flow_id` decimal(20,0) NOT NULL,
  `card_id` varchar(20) NOT NULL,
  `savelist_amt` decimal(18,4) DEFAULT NULL,
  `oper_type` varchar(1) DEFAULT NULL,
  `send_flag` varchar(1) DEFAULT '0',
  `oper_date` datetime DEFAULT NULL,
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_savelist`
--

LOCK TABLES `mess_t_vip_savelist` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_savelist` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_savelist` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mess_t_vip_type`
--

DROP TABLE IF EXISTS `mess_t_vip_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mess_t_vip_type` (
  `flow_id` decimal(20,0) NOT NULL,
  `card_id` varchar(20) NOT NULL,
  `card_type` varchar(2) NOT NULL,
  `to_card_type` varchar(2) NOT NULL,
  `send_flag` varchar(1) DEFAULT '0',
  `oper_date` datetime DEFAULT NULL,
  PRIMARY KEY (`flow_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mess_t_vip_type`
--

LOCK TABLES `mess_t_vip_type` WRITE;
/*!40000 ALTER TABLE `mess_t_vip_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `mess_t_vip_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pbcatcol`
--

DROP TABLE IF EXISTS `pbcatcol`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pbcatcol` (
  `pbc_tnam` varchar(30) DEFAULT NULL,
  `pbc_tid` int(11) DEFAULT NULL,
  `pbc_ownr` varchar(30) DEFAULT NULL,
  `pbc_cnam` varchar(30) DEFAULT NULL,
  `pbc_cid` smallint(6) DEFAULT NULL,
  `pbc_labl` varchar(254) DEFAULT NULL,
  `pbc_lpos` smallint(6) DEFAULT NULL,
  `pbc_hdr` varchar(254) DEFAULT NULL,
  `pbc_hpos` smallint(6) DEFAULT NULL,
  `pbc_jtfy` smallint(6) DEFAULT NULL,
  `pbc_mask` varchar(31) DEFAULT NULL,
  `pbc_case` smallint(6) DEFAULT NULL,
  `pbc_hght` smallint(6) DEFAULT NULL,
  `pbc_wdth` smallint(6) DEFAULT NULL,
  `pbc_ptrn` varchar(31) DEFAULT NULL,
  `pbc_bmap` varchar(1) DEFAULT NULL,
  `pbc_init` varchar(254) DEFAULT NULL,
  `pbc_cmnt` varchar(254) DEFAULT NULL,
  `pbc_edit` varchar(31) DEFAULT NULL,
  `pbc_tag` varchar(254) DEFAULT NULL,
  UNIQUE KEY `pbcatcol_idx` (`pbc_tnam`,`pbc_ownr`,`pbc_cnam`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pbcatcol`
--

LOCK TABLES `pbcatcol` WRITE;
/*!40000 ALTER TABLE `pbcatcol` DISABLE KEYS */;
INSERT INTO `pbcatcol` VALUES ('cey_u_handover_master         ',757577737,'dbo                           ','vch_accept                    ',5,'',0,'',0,0,'',0,0,0,'','N','','','','');
/*!40000 ALTER TABLE `pbcatcol` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pbcatedt`
--

DROP TABLE IF EXISTS `pbcatedt`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pbcatedt` (
  `pbe_name` varchar(30) NOT NULL,
  `pbe_edit` varchar(254) DEFAULT NULL,
  `pbe_type` smallint(6) NOT NULL,
  `pbe_cntr` int(11) DEFAULT NULL,
  `pbe_seqn` smallint(6) NOT NULL,
  `pbe_flag` int(11) DEFAULT NULL,
  `pbe_work` varchar(32) DEFAULT NULL,
  UNIQUE KEY `pbcatedt_idx` (`pbe_name`,`pbe_seqn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pbcatedt`
--

LOCK TABLES `pbcatedt` WRITE;
/*!40000 ALTER TABLE `pbcatedt` DISABLE KEYS */;
INSERT INTO `pbcatedt` VALUES ('#####','#####',90,1,1,32,'10                              '),('###,###.00','###,###.00',90,1,1,32,'10                              '),('###-##-####','###-##-####',90,1,1,32,'00                              '),('DD/MM/YY','DD/MM/YY',90,1,1,32,'20                              '),('DD/MM/YY HH:MM:SS','DD/MM/YY HH:MM:SS',90,1,1,32,'40                              '),('DD/MM/YY HH:MM:SS:FFFFFF','DD/MM/YY HH:MM:SS:FFFFFF',90,1,1,32,'40                              '),('DD/MM/YYYY','DD/MM/YYYY',90,1,1,32,'20                              '),('DD/MM/YYYY HH:MM:SS','DD/MM/YYYY HH:MM:SS',90,1,1,32,'40                              '),('DD/MMM/YY','DD/MMM/YY',90,1,1,32,'20                              '),('DD/MMM/YY HH:MM:SS','DD/MMM/YY HH:MM:SS',90,1,1,32,'40                              '),('HH:MM:SS','HH:MM:SS',90,1,1,32,'30                              '),('HH:MM:SS:FFF','HH:MM:SS:FFF',90,1,1,32,'30                              '),('HH:MM:SS:FFFFFF','HH:MM:SS:FFFFFF',90,1,1,32,'30                              '),('JJJ/YY','JJJ/YY',90,1,1,32,'20                              '),('JJJ/YY HH:MM:SS','JJJ/YY HH:MM:SS',90,1,1,32,'40                              '),('JJJ/YYYY','JJJ/YYYY',90,1,1,32,'20                              '),('JJJ/YYYY HH:MM:SS','JJJ/YYYY HH:MM:SS',90,1,1,32,'40                              '),('MM/DD/YY','MM/DD/YY',90,1,1,32,'20                              '),('MM/DD/YY HH:MM:SS','MM/DD/YY HH:MM:SS',90,1,1,32,'40                              '),('MM/DD/YYYY','MM/DD/YYYY',90,1,1,32,'20                              '),('MM/DD/YYYY HH:MM:SS','MM/DD/YYYY HH:MM:SS',90,1,1,32,'40                              ');
/*!40000 ALTER TABLE `pbcatedt` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pbcatfmt`
--

DROP TABLE IF EXISTS `pbcatfmt`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pbcatfmt` (
  `pbf_name` varchar(30) NOT NULL,
  `pbf_frmt` varchar(254) NOT NULL,
  `pbf_type` smallint(6) NOT NULL,
  `pbf_cntr` int(11) DEFAULT NULL,
  UNIQUE KEY `pbcatfmt_idx` (`pbf_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pbcatfmt`
--

LOCK TABLES `pbcatfmt` WRITE;
/*!40000 ALTER TABLE `pbcatfmt` DISABLE KEYS */;
INSERT INTO `pbcatfmt` VALUES ('#,##0','#,##0',81,0),('#,##0.00','#,##0.00',81,0),('$#,##0.00;($#,##0.00)','$#,##0.00;($#,##0.00)',81,0),('$#,##0.00;[RED]($#,##0.00)','$#,##0.00;[RED]($#,##0.00)',81,0),('$#,##0;($#,##0)','$#,##0;($#,##0)',81,0),('$#,##0;[RED]($#,##0)','$#,##0;[RED]($#,##0)',81,0),('0','0',81,0),('0%','0%',81,0),('0.00','0.00',81,0),('0.00%','0.00%',81,0),('0.00E+00','0.00E+00',81,0),('d-mmm','d-mmm',84,0),('d-mmm-yy','d-mmm-yy',84,0),('h:mm AM/PM','h:mm AM/PM',84,0),('h:mm:ss','h:mm:ss',84,0),('h:mm:ss AM/PM','h:mm:ss AM/PM',84,0),('m/d/yy','m/d/yy',84,0),('m/d/yy h:mm','m/d/yy h:mm',84,0),('mmm-yy','mmm-yy',84,0),('[General]','[General]',81,0);
/*!40000 ALTER TABLE `pbcatfmt` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pbcattbl`
--

DROP TABLE IF EXISTS `pbcattbl`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pbcattbl` (
  `pbt_tnam` varchar(30) DEFAULT NULL,
  `pbt_tid` int(11) DEFAULT NULL,
  `pbt_ownr` varchar(30) DEFAULT NULL,
  `pbd_fhgt` smallint(6) DEFAULT NULL,
  `pbd_fwgt` smallint(6) DEFAULT NULL,
  `pbd_fitl` varchar(1) DEFAULT NULL,
  `pbd_funl` varchar(1) DEFAULT NULL,
  `pbd_fchr` smallint(6) DEFAULT NULL,
  `pbd_fptc` smallint(6) DEFAULT NULL,
  `pbd_ffce` varchar(32) DEFAULT NULL,
  `pbh_fhgt` smallint(6) DEFAULT NULL,
  `pbh_fwgt` smallint(6) DEFAULT NULL,
  `pbh_fitl` varchar(1) DEFAULT NULL,
  `pbh_funl` varchar(1) DEFAULT NULL,
  `pbh_fchr` smallint(6) DEFAULT NULL,
  `pbh_fptc` smallint(6) DEFAULT NULL,
  `pbh_ffce` varchar(32) DEFAULT NULL,
  `pbl_fhgt` smallint(6) DEFAULT NULL,
  `pbl_fwgt` smallint(6) DEFAULT NULL,
  `pbl_fitl` varchar(1) DEFAULT NULL,
  `pbl_funl` varchar(1) DEFAULT NULL,
  `pbl_fchr` smallint(6) DEFAULT NULL,
  `pbl_fptc` smallint(6) DEFAULT NULL,
  `pbl_ffce` varchar(32) DEFAULT NULL,
  `pbt_cmnt` varchar(254) DEFAULT NULL,
  UNIQUE KEY `pbcattbl_idx` (`pbt_tnam`,`pbt_ownr`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pbcattbl`
--

LOCK TABLES `pbcattbl` WRITE;
/*!40000 ALTER TABLE `pbcattbl` DISABLE KEYS */;
INSERT INTO `pbcattbl` VALUES ('cey_u_handover_master         ',757577737,'dbo                           ',-12,400,'N','N',134,33,'System                          ',-12,400,'N','N',134,33,'System                          ',-12,400,'N','N',134,33,'System                          ','');
/*!40000 ALTER TABLE `pbcattbl` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pbcatvld`
--

DROP TABLE IF EXISTS `pbcatvld`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pbcatvld` (
  `pbv_name` varchar(30) NOT NULL,
  `pbv_vald` varchar(254) NOT NULL,
  `pbv_type` smallint(6) NOT NULL,
  `pbv_cntr` int(11) DEFAULT NULL,
  `pbv_msg` varchar(254) DEFAULT NULL,
  UNIQUE KEY `pbcatvld_idx` (`pbv_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pbcatvld`
--

LOCK TABLES `pbcatvld` WRITE;
/*!40000 ALTER TABLE `pbcatvld` DISABLE KEYS */;
/*!40000 ALTER TABLE `pbcatvld` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_bill`
--

DROP TABLE IF EXISTS `sc_bill`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_bill` (
  `no` varchar(255) NOT NULL COMMENT '流水号',
  `type` int(11) DEFAULT '1' COMMENT '单号类型',
  `inputDateTime` datetime DEFAULT NULL COMMENT '时间',
  `totalSalingPrice` double(10,0) DEFAULT NULL,
  `totalIncludeTax` double(255,0) DEFAULT NULL,
  `totalExcludeTax` double(255,0) DEFAULT NULL,
  `originalCost` double DEFAULT '0' COMMENT '总原价',
  `presentCost` double DEFAULT '0' COMMENT '现总价，应收',
  `actuallyPaid` double DEFAULT '0' COMMENT '实收 (应收减去收银优惠 抹零等)',
  `pos_id` int(255) DEFAULT NULL COMMENT 'POS机号',
  `pos_name` varchar(255) DEFAULT NULL,
  `customer_id` int(255) DEFAULT NULL COMMENT '客户',
  `customer_name` varchar(255) DEFAULT NULL,
  `cashier_id` int(255) DEFAULT NULL COMMENT '收银员',
  `cashier_name` varchar(255) DEFAULT NULL,
  `clerk_id` int(255) DEFAULT NULL COMMENT '营业员',
  `clerk_name` varchar(255) DEFAULT NULL,
  `member_no` varchar(255) DEFAULT NULL COMMENT '会员号',
  `member_name` varchar(255) DEFAULT NULL,
  `voucherNo` varchar(255) DEFAULT NULL COMMENT '交易凭证（用于微信 甩手掌柜等）',
  `remark` varchar(255) DEFAULT NULL,
  `abAccountIndex` int(11) DEFAULT '0' COMMENT 'AB帐',
  `state` int(11) DEFAULT '0' COMMENT '订单状态',
  `cloudState` int(11) DEFAULT '0' COMMENT '云上传状态',
  PRIMARY KEY (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_bill`
--

LOCK TABLES `sc_bill` WRITE;
/*!40000 ALTER TABLE `sc_bill` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_bill` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `billInsertTrigger` BEFORE INSERT ON `sc_bill`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `billUpdateTrigger` BEFORE UPDATE ON `sc_bill`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `sc_bill_item`
--

DROP TABLE IF EXISTS `sc_bill_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_bill_item` (
  `bill_no` varchar(255) DEFAULT NULL COMMENT '单号',
  `item_id` int(11) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `barcode` varchar(255) DEFAULT NULL COMMENT '条码',
  `name` varchar(255) DEFAULT NULL,
  `spec` varchar(255) DEFAULT NULL,
  `unit` varchar(255) DEFAULT NULL,
  `class_no` varchar(255) DEFAULT NULL,
  `ioFlag` int(11) DEFAULT NULL,
  `returnFlag` int(11) DEFAULT NULL,
  `discountType` int(11) DEFAULT '0' COMMENT '折扣类型(零售价 会员价 促销价 单品改价 整单折扣 单品折扣)',
  `quantity` double DEFAULT '1' COMMENT '数量',
  `salingPrice` double DEFAULT '0' COMMENT '现价',
  `salingTax` double DEFAULT NULL,
  `originalPrice` double DEFAULT NULL,
  `presentPrice` double DEFAULT NULL,
  `salingTaxTotal` double DEFAULT NULL,
  `originalPriceTotal` double DEFAULT NULL,
  `presentPriceTotal` double DEFAULT NULL,
  `remark` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_bill_item`
--

LOCK TABLES `sc_bill_item` WRITE;
/*!40000 ALTER TABLE `sc_bill_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_bill_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_bill_pay`
--

DROP TABLE IF EXISTS `sc_bill_pay`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_bill_pay` (
  `bill_no` varchar(255) DEFAULT NULL COMMENT '单号',
  `type` int(11) DEFAULT NULL COMMENT '类型',
  `amount` double DEFAULT NULL COMMENT '金额',
  `tradeNo` varchar(255) DEFAULT NULL COMMENT '特殊支付方式交易号',
  `remark` varchar(255) DEFAULT NULL COMMENT '备注'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_bill_pay`
--

LOCK TABLES `sc_bill_pay` WRITE;
/*!40000 ALTER TABLE `sc_bill_pay` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_bill_pay` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_business`
--

DROP TABLE IF EXISTS `sc_business`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_business` (
  `no` varchar(255) NOT NULL,
  `type` int(11) DEFAULT NULL,
  `amount` double DEFAULT NULL,
  `make_employee_id` int(255) DEFAULT NULL,
  `make_employee_name` varchar(255) DEFAULT NULL,
  `makeDateTime` datetime DEFAULT NULL,
  `review_employee_id` int(255) DEFAULT NULL,
  `review_employee_name` varchar(255) DEFAULT NULL,
  `reviewDateTime` datetime DEFAULT NULL,
  `target_id` int(11) DEFAULT NULL COMMENT '供应商 客户编号',
  `target_name` varchar(255) DEFAULT NULL COMMENT '供应商 客户姓名',
  `state` int(11) DEFAULT '0',
  `remark` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_business`
--

LOCK TABLES `sc_business` WRITE;
/*!40000 ALTER TABLE `sc_business` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_business` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_business_check_item`
--

DROP TABLE IF EXISTS `sc_business_check_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_business_check_item` (
  `business_no` varchar(255) NOT NULL,
  `item_id` int(11) DEFAULT NULL,
  `barcode` varchar(255) NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `spec` varchar(255) DEFAULT NULL,
  `unit` varchar(255) DEFAULT NULL,
  `class_no` int(11) DEFAULT NULL,
  `curInventory` double(255,0) DEFAULT NULL COMMENT '现库存',
  `oriInventory` double(255,0) DEFAULT NULL COMMENT '原库存',
  `quantity` double DEFAULT NULL COMMENT '增减数量',
  `price` double(10,0) DEFAULT NULL,
  `totalPrice` double(10,0) DEFAULT NULL COMMENT '增减小计',
  `remark` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_business_check_item`
--

LOCK TABLES `sc_business_check_item` WRITE;
/*!40000 ALTER TABLE `sc_business_check_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_business_check_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_business_stock_item`
--

DROP TABLE IF EXISTS `sc_business_stock_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_business_stock_item` (
  `business_no` varchar(255) NOT NULL,
  `item_id` int(11) DEFAULT NULL,
  `barcode` varchar(255) NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `spec` varchar(255) DEFAULT NULL,
  `unit` varchar(255) DEFAULT NULL,
  `class_no` varchar(255) DEFAULT NULL,
  `taxPoint` double DEFAULT NULL COMMENT '用于计算税',
  `quantity` double DEFAULT '0',
  `price` double DEFAULT '0',
  `tax` varchar(255) DEFAULT NULL,
  `totalPrice` double DEFAULT '0',
  `totalTax` varchar(255) DEFAULT NULL,
  `remark` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_business_stock_item`
--

LOCK TABLES `sc_business_stock_item` WRITE;
/*!40000 ALTER TABLE `sc_business_stock_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_business_stock_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_clerk`
--

DROP TABLE IF EXISTS `sc_clerk`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_clerk` (
  `no` int(11) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_clerk`
--

LOCK TABLES `sc_clerk` WRITE;
/*!40000 ALTER TABLE `sc_clerk` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_clerk` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_clerk_bonus`
--

DROP TABLE IF EXISTS `sc_clerk_bonus`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_clerk_bonus` (
  `clerk_no` int(11) DEFAULT NULL,
  `bill_no` int(11) DEFAULT NULL,
  `total` double DEFAULT NULL,
  `bonus` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_clerk_bonus`
--

LOCK TABLES `sc_clerk_bonus` WRITE;
/*!40000 ALTER TABLE `sc_clerk_bonus` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_clerk_bonus` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_customer`
--

DROP TABLE IF EXISTS `sc_customer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_customer` (
  `id` int(11) NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `phone` varchar(255) DEFAULT NULL,
  `address` varchar(255) DEFAULT NULL,
  `nationalID` varchar(255) DEFAULT NULL,
  `economyID` varchar(255) DEFAULT NULL,
  `remark` varchar(255) DEFAULT NULL,
  `state` int(11) DEFAULT NULL,
  `cloudState` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_customer`
--

LOCK TABLES `sc_customer` WRITE;
/*!40000 ALTER TABLE `sc_customer` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_customer` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_employee`
--

DROP TABLE IF EXISTS `sc_employee`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_employee` (
  `id` int(11) NOT NULL,
  `type` int(11) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `password` varchar(255) DEFAULT '',
  `discountDegree` double DEFAULT '100' COMMENT '最高折扣1~100',
  `remark` varchar(255) DEFAULT NULL,
  `buildDateTime` datetime DEFAULT NULL,
  `lastLoginDateTime` datetime DEFAULT NULL,
  `loginCount` int(11) DEFAULT '0',
  `state` int(11) DEFAULT '0',
  `cloudState` int(11) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_employee`
--

LOCK TABLES `sc_employee` WRITE;
/*!40000 ALTER TABLE `sc_employee` DISABLE KEYS */;
INSERT INTO `sc_employee` VALUES (1001,NULL,'默认员工','',90,'默认','1990-10-08 00:00:00','2015-12-22 17:59:10',1,0,0);
/*!40000 ALTER TABLE `sc_employee` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_employee_permission`
--

DROP TABLE IF EXISTS `sc_employee_permission`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_employee_permission` (
  `employee_id` int(11) DEFAULT NULL,
  `permissionID` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_employee_permission`
--

LOCK TABLES `sc_employee_permission` WRITE;
/*!40000 ALTER TABLE `sc_employee_permission` DISABLE KEYS */;
INSERT INTO `sc_employee_permission` VALUES (1001,201),(1001,202),(1001,203),(1001,204),(1001,205),(1001,206),(1001,301),(1001,302),(1001,303),(1001,304),(1001,305),(1001,306),(1001,307),(1001,308),(1001,309),(1001,310);
/*!40000 ALTER TABLE `sc_employee_permission` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_hand`
--

DROP TABLE IF EXISTS `sc_hand`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_hand` (
  `id` int(11) DEFAULT NULL,
  `inputDateTime` datetime DEFAULT NULL,
  `employee_id` int(11) DEFAULT NULL,
  `pos_id` int(11) DEFAULT NULL,
  `billCount` int(11) DEFAULT NULL,
  `originalCost` double DEFAULT NULL,
  `presentCost` double DEFAULT NULL,
  `actuallyPaid` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_hand`
--

LOCK TABLES `sc_hand` WRITE;
/*!40000 ALTER TABLE `sc_hand` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_hand` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_hand_item`
--

DROP TABLE IF EXISTS `sc_hand_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_hand_item` (
  `hand_id` int(11) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `amount` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_hand_item`
--

LOCK TABLES `sc_hand_item` WRITE;
/*!40000 ALTER TABLE `sc_hand_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_hand_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_item`
--

DROP TABLE IF EXISTS `sc_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_item` (
  `id` int(11) NOT NULL,
  `barcode` varchar(50) DEFAULT NULL COMMENT '条码',
  `type` int(11) DEFAULT '1' COMMENT '商品类型(1 2 3)',
  `name` varchar(255) DEFAULT NULL COMMENT '名称',
  `pinyin` varchar(50) DEFAULT NULL COMMENT '拼音',
  `spec` varchar(50) DEFAULT NULL COMMENT '规格',
  `unit` varchar(50) DEFAULT NULL COMMENT '单位',
  `productionPlace` varchar(50) DEFAULT NULL,
  `class_no` varchar(50) DEFAULT '0' COMMENT '分类',
  `brand_id` int(11) DEFAULT '0' COMMENT '品牌id',
  `supplier_id` int(11) DEFAULT NULL,
  `lastestStockPrice` double DEFAULT '0' COMMENT '最新进价',
  `meanStockPrice` double DEFAULT '0' COMMENT '平均进价',
  `salePrice` double DEFAULT '0' COMMENT '销售价',
  `memberPrice` double DEFAULT '0' COMMENT '会员价',
  `taxPoint` double DEFAULT '0' COMMENT '税点',
  `taxPoint2` double DEFAULT '0',
  `taxPoint3` double DEFAULT '0',
  `taxType` int(11) DEFAULT NULL,
  `returnFlag` int(11) DEFAULT NULL,
  `weighFlag` int(11) DEFAULT '0' COMMENT '称重标志',
  `discountFlag` int(11) DEFAULT '1' COMMENT '打折标志',
  `pointFlag` int(11) DEFAULT '1' COMMENT '积分标志',
  `inventory` double DEFAULT '0' COMMENT '库存',
  `inventoryMax` double DEFAULT '0' COMMENT '最大库存',
  `inventoryMin` double DEFAULT '0' COMMENT '最低库存',
  `clerkBonusType` int(11) DEFAULT '0' COMMENT '营业员提成类型',
  `clerkBonusValue` double DEFAULT '0' COMMENT '提成数量',
  `combine` varchar(255) DEFAULT NULL COMMENT '组合商品',
  `ownCode` varchar(255) DEFAULT NULL COMMENT '自编码',
  `totalPurchaseQuantity` double DEFAULT '0' COMMENT '进货总数量',
  `totalPuchaseAmount` double DEFAULT '0' COMMENT '进货总金额',
  `state` int(11) DEFAULT '0' COMMENT '商品状态',
  `cloudState` int(11) DEFAULT '0' COMMENT '上传标志',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_item`
--

LOCK TABLES `sc_item` WRITE;
/*!40000 ALTER TABLE `sc_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_item` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `itemInsertTrigger` BEFORE INSERT ON `sc_item`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `itemUpdateTrigger` BEFORE UPDATE ON `sc_item`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `sc_item_brand`
--

DROP TABLE IF EXISTS `sc_item_brand`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_item_brand` (
  `id` int(11) NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_item_brand`
--

LOCK TABLES `sc_item_brand` WRITE;
/*!40000 ALTER TABLE `sc_item_brand` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_item_brand` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_item_class`
--

DROP TABLE IF EXISTS `sc_item_class`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_item_class` (
  `no` varchar(255) NOT NULL COMMENT '自己编号',
  `parent_no` varchar(255) DEFAULT '1' COMMENT '父类编号',
  `name` varchar(255) DEFAULT NULL COMMENT '姓名',
  `discount` double DEFAULT NULL,
  `pointEveryMoney` double DEFAULT '0' COMMENT '每元积分数',
  `state` int(11) DEFAULT '0',
  `cloudState` double DEFAULT '0' COMMENT '云标志',
  PRIMARY KEY (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_item_class`
--

LOCK TABLES `sc_item_class` WRITE;
/*!40000 ALTER TABLE `sc_item_class` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_item_class` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `sortInsertTrigger` BEFORE INSERT ON `sc_item_class`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `sortUpdateTrigger` BEFORE UPDATE ON `sc_item_class`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `sc_item_inventory`
--

DROP TABLE IF EXISTS `sc_item_inventory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_item_inventory` (
  `no` varchar(255) DEFAULT NULL,
  `ioDateTime` datetime DEFAULT NULL,
  `ioFlag` int(11) DEFAULT '1' COMMENT '1为进货，-1为出货,为了计算数量金额',
  `type` int(11) DEFAULT NULL,
  `item_id` int(11) DEFAULT NULL,
  `barcode` varchar(255) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `spec` varchar(255) DEFAULT NULL,
  `unit` varchar(255) DEFAULT NULL,
  `class_no` varchar(11) DEFAULT NULL,
  `quantity` double DEFAULT NULL COMMENT '正数',
  `price` double DEFAULT NULL,
  `total` double DEFAULT NULL COMMENT '正数',
  `remark` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_item_inventory`
--

LOCK TABLES `sc_item_inventory` WRITE;
/*!40000 ALTER TABLE `sc_item_inventory` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_item_inventory` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_item_unit`
--

DROP TABLE IF EXISTS `sc_item_unit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_item_unit` (
  `id` int(11) NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `remark` varchar(255) DEFAULT NULL,
  `cloudState` int(11) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_item_unit`
--

LOCK TABLES `sc_item_unit` WRITE;
/*!40000 ALTER TABLE `sc_item_unit` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_item_unit` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `unitInsertTrigger` BEFORE INSERT ON `sc_item_unit`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `unitUpdateTrigger` BEFORE UPDATE ON `sc_item_unit`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `sc_member`
--

DROP TABLE IF EXISTS `sc_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_member` (
  `no` varchar(255) NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `password` varchar(255) DEFAULT NULL,
  `type_id` int(11) DEFAULT NULL,
  `birthday` date DEFAULT NULL,
  `phone` varchar(255) DEFAULT NULL,
  `cardNo` varchar(255) DEFAULT NULL,
  `deposit` double DEFAULT '0',
  `point` double DEFAULT '0',
  `remark` varchar(255) DEFAULT NULL,
  `state` int(11) DEFAULT NULL,
  `cloudState` int(11) DEFAULT NULL,
  PRIMARY KEY (`no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_member`
--

LOCK TABLES `sc_member` WRITE;
/*!40000 ALTER TABLE `sc_member` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_member` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `memberInsertTrigger` BEFORE INSERT ON `sc_member`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `memberUpdateTrigger` BEFORE UPDATE ON `sc_member`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `sc_member_deposit`
--

DROP TABLE IF EXISTS `sc_member_deposit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_member_deposit` (
  `no` varchar(255) DEFAULT NULL,
  `ioDateTime` datetime DEFAULT NULL,
  `ioFlag` int(11) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `member_no` varchar(255) DEFAULT NULL,
  `amount` double DEFAULT NULL,
  `remark` varchar(255) DEFAULT NULL COMMENT '写入充值付款多少'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_member_deposit`
--

LOCK TABLES `sc_member_deposit` WRITE;
/*!40000 ALTER TABLE `sc_member_deposit` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_member_deposit` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_member_gift`
--

DROP TABLE IF EXISTS `sc_member_gift`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_member_gift` (
  `id` int(11) NOT NULL,
  `item_id` int(11) DEFAULT NULL,
  `barcode` varchar(255) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `point` int(255) DEFAULT NULL,
  `beginDate` date DEFAULT NULL,
  `endDate` date DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_member_gift`
--

LOCK TABLES `sc_member_gift` WRITE;
/*!40000 ALTER TABLE `sc_member_gift` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_member_gift` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_member_point`
--

DROP TABLE IF EXISTS `sc_member_point`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_member_point` (
  `no` varchar(255) DEFAULT NULL,
  `ioDateTime` datetime DEFAULT NULL,
  `ioFlag` int(11) DEFAULT NULL,
  `type` int(11) DEFAULT NULL COMMENT '积分增减 积分兑换 销售积分增加',
  `member_no` varchar(255) DEFAULT NULL,
  `amount` double DEFAULT NULL,
  `remark` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_member_point`
--

LOCK TABLES `sc_member_point` WRITE;
/*!40000 ALTER TABLE `sc_member_point` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_member_point` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_member_type`
--

DROP TABLE IF EXISTS `sc_member_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_member_type` (
  `id` int(255) NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `discountType` int(11) DEFAULT NULL,
  `discount` double DEFAULT NULL,
  `depositFlag` int(11) DEFAULT NULL,
  `pointFlag` int(11) DEFAULT NULL,
  `pointRatio` double DEFAULT NULL COMMENT '积分倍数',
  `remark` varchar(255) DEFAULT NULL,
  `cloudState` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_member_type`
--

LOCK TABLES `sc_member_type` WRITE;
/*!40000 ALTER TABLE `sc_member_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_member_type` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `memberTypeInsertTrigger` BEFORE INSERT ON `sc_member_type`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `memberTypeUpdateTrigger` BEFORE UPDATE ON `sc_member_type`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `sc_promotion`
--

DROP TABLE IF EXISTS `sc_promotion`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_promotion` (
  `id` int(11) NOT NULL,
  `type` int(11) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `beginDate` date DEFAULT NULL,
  `endDate` date DEFAULT NULL,
  `beginTime` time DEFAULT NULL,
  `endTime` time DEFAULT NULL,
  `mondayFlag` int(11) DEFAULT NULL,
  `tuesdayFlag` int(11) DEFAULT NULL,
  `wednesdayFlag` int(11) DEFAULT NULL,
  `thursdayFlag` int(11) DEFAULT NULL,
  `fridayFlag` int(11) DEFAULT NULL,
  `saturdayFlag` int(11) DEFAULT NULL,
  `sundayFlag` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_promotion`
--

LOCK TABLES `sc_promotion` WRITE;
/*!40000 ALTER TABLE `sc_promotion` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_promotion` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_promotion_item`
--

DROP TABLE IF EXISTS `sc_promotion_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_promotion_item` (
  `promotion_id` int(11) DEFAULT NULL,
  `item_id` int(11) DEFAULT NULL,
  `barcode` varchar(255) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `salePrice` double DEFAULT NULL,
  `memberPrice` double DEFAULT NULL,
  `price` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_promotion_item`
--

LOCK TABLES `sc_promotion_item` WRITE;
/*!40000 ALTER TABLE `sc_promotion_item` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_promotion_item` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_supplier`
--

DROP TABLE IF EXISTS `sc_supplier`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_supplier` (
  `id` int(255) NOT NULL COMMENT '编号',
  `name` varchar(255) DEFAULT NULL COMMENT '姓名',
  `pinyin` varchar(255) DEFAULT NULL COMMENT '助记码',
  `linkman` varchar(255) DEFAULT NULL COMMENT '联系人',
  `phone` varchar(255) DEFAULT NULL COMMENT '联系人电话',
  `address` varchar(255) DEFAULT NULL COMMENT '地址',
  `nationalID` varchar(255) DEFAULT NULL,
  `economyID` varchar(255) DEFAULT NULL,
  `remark` varchar(255) DEFAULT NULL COMMENT '备注',
  `state` int(11) DEFAULT NULL COMMENT '状态',
  `cloudState` int(11) DEFAULT '0' COMMENT '云标志',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_supplier`
--

LOCK TABLES `sc_supplier` WRITE;
/*!40000 ALTER TABLE `sc_supplier` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_supplier` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `supplierInsertTrigger` BEFORE INSERT ON `sc_supplier`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `supplierUpdateTrigger` BEFORE UPDATE ON `sc_supplier`
FOR EACH ROW Begin IF new.cloudState = -1 then set new.cloudState = 1; ELSE set new.cloudState = 0; END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `sc_system_configure`
--

DROP TABLE IF EXISTS `sc_system_configure`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_system_configure` (
  `name` varchar(255) NOT NULL,
  `value` varchar(255) DEFAULT NULL,
  `flag` int(11) DEFAULT NULL,
  `remark` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_system_configure`
--

LOCK TABLES `sc_system_configure` WRITE;
/*!40000 ALTER TABLE `sc_system_configure` DISABLE KEYS */;
INSERT INTO `sc_system_configure` VALUES ('system_verson','2.4',NULL,NULL);
/*!40000 ALTER TABLE `sc_system_configure` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sc_system_pos`
--

DROP TABLE IF EXISTS `sc_system_pos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sc_system_pos` (
  `id` int(255) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `registDateTime` datetime DEFAULT NULL,
  `remark` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sc_system_pos`
--

LOCK TABLES `sc_system_pos` WRITE;
/*!40000 ALTER TABLE `sc_system_pos` DISABLE KEYS */;
/*!40000 ALTER TABLE `sc_system_pos` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_m_deposit`
--

DROP TABLE IF EXISTS `t_m_deposit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_deposit` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `vch_memberno` varchar(20) NOT NULL,
  `num_deposit` decimal(12,2) DEFAULT NULL,
  `num_realamount` decimal(12,2) DEFAULT NULL,
  `ch_deposit_mode` varchar(1) DEFAULT NULL,
  `ch_pay_mode` varchar(1) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `remain_amt` decimal(17,2) DEFAULT '0.00',
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `terminal_code` varchar(128) DEFAULT '',
  PRIMARY KEY (`int_flowID`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_deposit`
--

LOCK TABLES `t_m_deposit` WRITE;
/*!40000 ALTER TABLE `t_m_deposit` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_deposit` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER `tr_m_deposit_send` AFTER INSERT ON `t_m_deposit` FOR EACH ROW BEGIN
  DECLARE ls_cardid char(20);  
  DECLARE ln_consum_amt numeric(16,2); 
  DECLARE ls_oper_type char(1);
  DECLARE ls_var_value char(255);
  DECLARE ls_branch_no varchar(10);
  DECLARE g_branch_no varchar(10);
	DECLARE deposit_mode char(1);
	DECLARE ln_point numeric(16,2);
	DECLARE ls_memo	varchar(100);
	DECLARE ll_posit int;

	set deposit_mode = NEW.ch_deposit_mode ;

	IF (deposit_mode = '1' or deposit_mode = '2' or deposit_mode = '7') THEN

	 set ls_branch_no = '0000';

	 IF (deposit_mode = '1') THEN
		
			set ls_var_value = (select sys_var_value  from mess_t_sys_system where Upper(LTRIM(RTRIM(sys_var_id))) = Upper(LTRIM(RTRIM('card_save'))));
			set ls_cardid = NEW.vch_memberno ;
      set ln_consum_amt = NEW.num_deposit;
      set ls_memo = NEW.vch_memo;
				
			IF (length(ls_var_value) <> 0 and ifnull(ls_var_value,'') <> '') THEN
		    IF (left(ls_var_value,1) = '1') THEN
			       insert into mess_t_vip_message(mess_type,branch_no,card_id,savelist_amt,oper_type,send_flag)  values('4',ls_branch_no,ls_cardid,ln_consum_amt,'4','0');
        END IF;
      END IF;
		

	ELSEIF (deposit_mode = '7') THEN
			
		   set ls_var_value = (select ifnull(sys_var_value,'')  FROM mess_t_sys_system where Upper(LTRIM(RTRIM(sys_var_id))) = Upper(LTRIM(RTRIM('vip_giv6'))));				
       IF (length(ls_var_value) <> 0 and ifnull(ls_var_value,'') <> '') THEN
		    	  IF ( left(ls_var_value,1) = '1' ) THEN
					      set ls_cardid = NEW.vch_memberno ;
					      set ln_consum_amt = - NEW.num_deposit;
					      set ls_memo = NEW.vch_memo ;

					     IF (ls_memo is null or ls_memo = '') THEN
						     set ln_point = 0;
					     ELSE
							   set ll_posit = charindex ( ':',ls_memo,0 );
							   set ln_point = format(round( ls_memo, len( ls_memo ) - ll_posit ),2);
               END IF;

					     insert into mess_t_vip_message(mess_type,branch_no,card_id,father_card,giv_acc_num,send_flag,oper_type,change_amt) 
					      values('2',ls_branch_no,ls_cardid,ls_cardid,ln_consum_amt,'0','6',ln_point);
            END IF;
       END IF;
			
END IF;

END IF;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `t_m_gift`
--

DROP TABLE IF EXISTS `t_m_gift`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_gift` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `ch_giftno` varchar(4) NOT NULL,
  `vch_giftname` varchar(30) NOT NULL,
  `num_point` decimal(12,2) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_gift`
--

LOCK TABLES `t_m_gift` WRITE;
/*!40000 ALTER TABLE `t_m_gift` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_gift` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_gift_insert AFTER INSERT ON t_m_gift FOR EACH ROW  Begin  delete from del_t_m_gift where int_flowID=NEW.int_flowID; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_gift_update BEFORE UPDATE ON t_m_gift FOR EACH ROW  Begin   IF(NEW.int_flowID = OLD.int_flowID and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_gift_delete BEFORE DELETE ON t_m_gift FOR EACH ROW  Begin  replace into del_t_m_gift select * from t_m_gift where int_flowID=OLD.int_flowID; update del_t_m_gift set ch_yun_already_keep = 'N' where int_flowID=OLD.int_flowID; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `t_m_hang_balance`
--

DROP TABLE IF EXISTS `t_m_hang_balance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_hang_balance` (
  `ch_balanceno` varchar(15) NOT NULL DEFAULT '',
  `vch_memberno` varchar(20) NOT NULL,
  `dt_date` datetime DEFAULT NULL,
  `num_remain` decimal(20,2) DEFAULT NULL,
  `num_pay` decimal(20,2) DEFAULT NULL,
  `num_amount` decimal(20,2) DEFAULT NULL,
  `vch_balance` varchar(16) DEFAULT NULL,
  `vch_operId` varchar(16) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `ch_state` varchar(1) NOT NULL,
  `ch_type` varchar(1) NOT NULL,
  `ch_paymodeno` varchar(2) DEFAULT NULL,
  PRIMARY KEY (`ch_balanceno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_hang_balance`
--

LOCK TABLES `t_m_hang_balance` WRITE;
/*!40000 ALTER TABLE `t_m_hang_balance` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_hang_balance` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_m_member_log`
--

DROP TABLE IF EXISTS `t_m_member_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_member_log` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `vch_memberno` varchar(20) NOT NULL,
  `vch_matter` varchar(20) NOT NULL,
  `vch_memo` varchar(50) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `num_bace` decimal(20,2) DEFAULT '0.00',
  `dt_send_date` datetime DEFAULT NULL,
  `vch_member` varchar(40) DEFAULT NULL,
  `ch_typeno` varchar(2) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_member_log`
--

LOCK TABLES `t_m_member_log` WRITE;
/*!40000 ALTER TABLE `t_m_member_log` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_member_log` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_m_member_other`
--

DROP TABLE IF EXISTS `t_m_member_other`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_member_other` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `vch_memberno` varchar(20) NOT NULL,
  `vch_item` varchar(20) NOT NULL,
  `vch_memo` varchar(50) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_member_other`
--

LOCK TABLES `t_m_member_other` WRITE;
/*!40000 ALTER TABLE `t_m_member_other` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_member_other` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_m_member_type`
--

DROP TABLE IF EXISTS `t_m_member_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_member_type` (
  `ch_typeno` varchar(2) NOT NULL,
  `vch_typename` varchar(16) NOT NULL,
  `ch_attribute` varchar(1) NOT NULL,
  `ch_favourable` varchar(1) DEFAULT NULL,
  `ch_pricekind` varchar(1) DEFAULT NULL,
  `int_discount` int(11) DEFAULT NULL,
  `ch_projectno` varchar(3) DEFAULT NULL,
  `ch_calc` varchar(1) DEFAULT NULL,
  `ch_calc_type` varchar(1) DEFAULT NULL,
  `int_calc_unit` int(11) DEFAULT NULL,
  `ch_area_flag` varchar(1) DEFAULT NULL,
  `int_backcash_rate` int(11) DEFAULT NULL,
  `int_card_baseamount` int(11) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`ch_typeno`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_member_type`
--

LOCK TABLES `t_m_member_type` WRITE;
/*!40000 ALTER TABLE `t_m_member_type` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_member_type` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_member_type_insert AFTER INSERT ON t_m_member_type FOR EACH ROW  Begin  delete from del_t_m_member_type where ch_typeno=NEW.ch_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_member_type_update BEFORE UPDATE ON t_m_member_type FOR EACH ROW  Begin   IF(NEW.ch_typeno = OLD.ch_typeno and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER tr_web_t_m_member_type_delete BEFORE DELETE ON t_m_member_type FOR EACH ROW  Begin  replace into del_t_m_member_type select * from t_m_member_type where ch_typeno=OLD.ch_typeno; update del_t_m_member_type set ch_yun_already_keep = 'N' where ch_typeno=OLD.ch_typeno; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `t_m_parameter`
--

DROP TABLE IF EXISTS `t_m_parameter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_parameter` (
  `vch_parameter` varchar(20) NOT NULL,
  `vch_value` varchar(30) DEFAULT NULL,
  `vch_explain` varchar(30) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`vch_parameter`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_parameter`
--

LOCK TABLES `t_m_parameter` WRITE;
/*!40000 ALTER TABLE `t_m_parameter` DISABLE KEYS */;
INSERT INTO `t_m_parameter` VALUES ('deposit_pay_all','0','充值卡付款必须全单满付',NULL),('discount_view','0','折扣时显示会员折扣详情',NULL),('input_memberno','1','可手动输入会员号',NULL),('local_member_select','0','启用本地会员营业时查卡',NULL),('member_ATT_mode','123','会员需要使用类型',NULL),('member_bank_print','0','会员作废的时候打印账单',NULL),('m_card_paydiscount','0','充值卡会员付款时自动折扣',NULL),('m_deposit_negative','0','充值与实收的金额必须相等',NULL),('m_fetch_base','0','取款可取保本金额',NULL),('m_max_deposit','30,000.00','最大充值金额',NULL),('m_member_pay','0','充值会员付款允许负数',NULL),('vip_card_type','磁卡','卡类型',NULL),('vip_ic_baud','','IC卡波特率',NULL),('vip_ic_machine','明华RF','IC卡读写器',NULL),('vip_ic_password','FFFFFF','IC卡读写密码',NULL),('vip_ic_port','USB','读写器接口',NULL),('vip_phone_verify','1','会员付款短信验证','N');
/*!40000 ALTER TABLE `t_m_parameter` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'IGNORE_SPACE' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`127.0.0.1`*/ /*!50003 TRIGGER tr_web_t_m_parameter_update BEFORE UPDATE ON t_m_parameter FOR EACH ROW  Begin   IF(NEW.vch_parameter = OLD.vch_parameter and NEW.ch_yun_already_keep = '1') THEN set NEW.ch_yun_already_keep = 'Y';   ELSE  set NEW.ch_yun_already_keep = 'N';  END IF; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `t_m_pay`
--

DROP TABLE IF EXISTS `t_m_pay`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_pay` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `vch_memberno` varchar(20) NOT NULL,
  `ch_payno` varchar(15) DEFAULT NULL,
  `ch_paymodeno` varchar(2) DEFAULT NULL,
  `num_amount` decimal(12,2) DEFAULT NULL,
  `num_pay` decimal(12,2) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_balanceno` varchar(15) DEFAULT NULL,
  `int_checkflow` int(11) DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `terminal_code` varchar(128) DEFAULT '',
  PRIMARY KEY (`int_flowID`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_pay`
--

LOCK TABLES `t_m_pay` WRITE;
/*!40000 ALTER TABLE `t_m_pay` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_pay` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER `tr_m_pay_send` AFTER INSERT ON `t_m_pay` FOR EACH ROW BEGIN

  DECLARE ls_cardid char(20);       
  DECLARE ln_consum_amt numeric(16,4);  
  DECLARE ls_oper_type char(1);
  DECLARE ls_var_value char(255);
  DECLARE ls_branch_no varchar(10);
  DECLARE g_branch_no varchar(10);

	set ls_branch_no = '0000';

  set ls_var_value = (select sys_var_value from mess_t_sys_system where Upper(LTRIM(RTRIM(sys_var_id))) = Upper(LTRIM(RTRIM('savecard_pay'))));
			
	IF (length(ls_var_value) <> 0 and ifnull(ls_var_value,'') <> '') THEN

      IF (left(ls_var_value,1) = '1') THEN
     
		     set ls_cardid = NEW.vch_memberno ;
         set ln_consum_amt = NEW.num_pay ;
	
	       insert into mess_t_vip_message(mess_type,branch_no,card_id,savelist_amt,oper_type,send_flag) 
	         values('4',ls_branch_no,ls_cardid,ln_consum_amt,'5','0');
      END IF;
	
  END IF; 

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `t_m_point`
--

DROP TABLE IF EXISTS `t_m_point`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_point` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `vch_memberno` varchar(20) NOT NULL,
  `ch_type` varchar(1) NOT NULL,
  `ch_payno` varchar(15) DEFAULT NULL,
  `num_point` decimal(12,2) DEFAULT NULL,
  `vch_memo` varchar(50) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_yun_already_keep` varchar(1) DEFAULT NULL,
  `terminal_code` varchar(128) DEFAULT '',
  PRIMARY KEY (`int_flowID`),
  KEY `index_name` (`ch_yun_already_keep`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_point`
--

LOCK TABLES `t_m_point` WRITE;
/*!40000 ALTER TABLE `t_m_point` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_point` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`%`*/ /*!50003 TRIGGER `tr_m_point_send` AFTER INSERT ON `t_m_point` FOR EACH ROW BEGIN

  DECLARE ls_cardid 		char(20);      
  DECLARE ln_consum_amt 		numeric(16,4);  
  DECLARE ln_consum_num 		numeric(16,4);  
  DECLARE ls_oper_type 		char(1);
  DECLARE ls_var_value 		char(255);
  DECLARE ls_branch_no		varchar(10);
  DECLARE g_branch_no 		varchar(10);
  DECLARE jzbillno	 		varchar(20);
  DECLARE type		    	char(1);

      
	set ls_cardid = NEW.vch_memberno ;
  set ln_consum_num = NEW.num_point;
  set jzbillno = NEW.ch_payno ;
  set type = NEW.ch_type ;

	set ls_branch_no = '0000';

	IF (type = '2' or type = '1'  or type = '3') THEN

			  set ls_var_value = (select sys_var_value from mess_t_sys_system where Upper(LTRIM(RTRIM(sys_var_id))) = Upper(LTRIM(RTRIM('consumer_auto'))));
					
				IF (length(ls_var_value) <> 0 and ifnull(ls_var_value,'') <> '') THEN
				  IF (left(ls_var_value,1) = '1') THEN
				    IF (jzbillno is not null and jzbillno <> '') THEN
				        set ln_consum_amt = (select num_cost from cey_u_checkout_master where ch_payno = jzbillno); 
            END IF;
						
				   insert into mess_t_vip_message(mess_type,branch_no,card_id,acc_num,flow_amt,oper_type,send_flag,flow_no)
		 		   values ('5',ls_branch_no,ls_cardid,ln_consum_num,ln_consum_amt,'0','0',jzbillno);
          END IF;
			  END IF;
	ELSEIF (type = '4') THEN 
				set ls_var_value = (select ifnull(sys_var_value,'')  FROM mess_t_sys_system WHERE Upper(LTRIM(RTRIM(sys_var_id))) = Upper(LTRIM(RTRIM('vip_giv3'))));
				IF (left(ls_var_value,1) = '1' ) THEN
				  insert into mess_t_vip_message(mess_type,branch_no,card_id,father_card,giv_acc_num,send_flag,oper_type) 
				     values('2',ls_branch_no,ls_cardid,ls_cardid,ln_consum_num,'0','3');
        END IF;
			
  END IF;

END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `t_m_point_gift`
--

DROP TABLE IF EXISTS `t_m_point_gift`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_point_gift` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `vch_memberno` varchar(20) NOT NULL,
  `ch_giftname` varchar(30) DEFAULT NULL,
  `vch_operID` varchar(6) DEFAULT NULL,
  `num_point` decimal(12,2) DEFAULT NULL,
  `vch_memo` varchar(60) DEFAULT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `ch_branchno` varchar(4) DEFAULT NULL,
  `ch_upflag` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_point_gift`
--

LOCK TABLES `t_m_point_gift` WRITE;
/*!40000 ALTER TABLE `t_m_point_gift` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_point_gift` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_m_rw_password_log`
--

DROP TABLE IF EXISTS `t_m_rw_password_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_m_rw_password_log` (
  `int_flowID` int(11) NOT NULL AUTO_INCREMENT,
  `vch_operID` varchar(6) NOT NULL,
  `dt_operdate` datetime DEFAULT NULL,
  `vch_password_old` varchar(10) DEFAULT NULL,
  `vch_password_new` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`int_flowID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_m_rw_password_log`
--

LOCK TABLES `t_m_rw_password_log` WRITE;
/*!40000 ALTER TABLE `t_m_rw_password_log` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_m_rw_password_log` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_ms_comm_used`
--

DROP TABLE IF EXISTS `t_ms_comm_used`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ms_comm_used` (
  `id` int(11) NOT NULL,
  `mess_title` varchar(50) DEFAULT NULL,
  `mess_content` text,
  `mess_type` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_ms_comm_used`
--

LOCK TABLES `t_ms_comm_used` WRITE;
/*!40000 ALTER TABLE `t_ms_comm_used` DISABLE KEYS */;
INSERT INTO `t_ms_comm_used` VALUES (1,'入会得礼品','明媚春天，特别惊喜！举办大型新品推荐活动：邀请会员您携朋友共同参加可领取礼品！时间4 月7 日至10 日，美丽预定热线：XXXX','营销类'),(2,'新店开业信息','尊敬的会员：购买不参加返券活动的化妆品现金满100 元可参加抽奖活动，百分百中奖，奖品最小为面值40 元百货券，惊喜多多！','营销类'),(3,'抽奖促销','4 月8 日至4 月17 日酬宾满200 送100，部分满200 送60 详情请登陆本商场网站：www.xxxx.com，本信息免费','营销类'),(4,'消费满多少参加抽奖','4 月20 日至5 月8 日酬宾满200 送100，部分满200 送60，5 月8 日母亲节有特色活动，详见店内明示.','营销类'),(5,'新品牌推广活动','XX 是充满青春活力的衍生品牌，近期新开店及优惠活动如下：17 日在新品牌隆重开业；此货品购物488，赠精美T恤一件.','营销类'),(6,'店庆促销','即日起至5 月7 日，凡至购买产品满720 元可获赠精美晶莹嫩白礼包一套，购满750 元可额外获赠专业化妆镜一面。','营销类'),(7,'五一促销','五一大酬宾，4 月28 日－5 月7 日化妆品买100 送58；300 元入会会员独享吉祥物好礼；日替换购先到先得。欢迎光临！','营销类'),(8,'端午节祝福','粽子香，香厨房；艾叶香，香满堂；桃枝插在大门上，出门一望麦儿黄；这儿端阳，那儿端阳，处处都端阳。祝福亲爱的朋友端午节快乐','祝福类'),(9,'中秋节祝福','千里试问平安否？且把思念遥相寄。绵绵爱意与关怀，浓浓情意与祝福，中秋快乐！','祝福类'),(10,'生日祝福','亲爱的#cust#sex，您的生日将至，诚邀您携带身份证和会员卡至领取精美手机链并享受一次免费肌肤护理。','营销类'),(11,'新会员礼品','欢迎您成为我们的会员。会员专享杂志《花草语汇》已为您寄出，携此短信至徐家汇形象专柜，可获赠精美体验装一份.','客服类'),(12,'会员奖励','您的会员卡内有新奖励金额为150 元，有效期至2005 年10 月，欢迎您再次光临！','客服类'),(13,'积分兑换','亲爱的#cust会员：售后中心推出积分兑换免费护理活动。详情请咨询.','客服类'),(14,'通知会员领卡','您好！你的会员卡已经做好，请您凭身份证和临时卡到领取。咨询电话：XXXXXX.','客服类');
/*!40000 ALTER TABLE `t_ms_comm_used` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_ms_cust_info`
--

DROP TABLE IF EXISTS `t_ms_cust_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ms_cust_info` (
  `group_no` varchar(4) NOT NULL,
  `cust_no` varchar(20) NOT NULL,
  `cust_name` varchar(20) DEFAULT NULL,
  `mobile` varchar(24) DEFAULT NULL,
  `sex` varchar(2) DEFAULT NULL,
  `addr` varchar(50) DEFAULT NULL,
  `zip` varchar(10) DEFAULT NULL,
  `phone` varchar(20) DEFAULT NULL,
  `e_mail` varchar(40) DEFAULT NULL,
  `birthday` datetime DEFAULT NULL,
  `company` varchar(50) DEFAULT NULL,
  `duty` varchar(20) DEFAULT NULL,
  `region` varchar(20) DEFAULT NULL,
  `id_card` varchar(20) DEFAULT NULL,
  `cust_type` varchar(20) DEFAULT '0',
  `begin_date` datetime DEFAULT NULL,
  `end_date` datetime DEFAULT NULL,
  `memo` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`cust_no`),
  KEY `i_bd_supcust_item_1` (`cust_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_ms_cust_info`
--

LOCK TABLES `t_ms_cust_info` WRITE;
/*!40000 ALTER TABLE `t_ms_cust_info` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_ms_cust_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_ms_group_info`
--

DROP TABLE IF EXISTS `t_ms_group_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ms_group_info` (
  `group_no` varchar(4) NOT NULL,
  `group_name` varchar(20) DEFAULT NULL,
  `type_flag` varchar(2) DEFAULT '0',
  `memo` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`group_no`),
  KEY `i_bd_supcust_item_1` (`group_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_ms_group_info`
--

LOCK TABLES `t_ms_group_info` WRITE;
/*!40000 ALTER TABLE `t_ms_group_info` DISABLE KEYS */;
INSERT INTO `t_ms_group_info` VALUES ('01  ','会员','1 ',NULL),('02  ','储值用户','1 ',NULL),('03  ','供应商','1 ',NULL),('04  ','客户','1 ',NULL);
/*!40000 ALTER TABLE `t_ms_group_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_ms_mess_flow`
--

DROP TABLE IF EXISTS `t_ms_mess_flow`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ms_mess_flow` (
  `flow_id` decimal(20,0) NOT NULL,
  `cust_no` varchar(20) DEFAULT NULL,
  `group_no` varchar(4) DEFAULT NULL,
  `mobile` varchar(20) DEFAULT NULL,
  `oper_id` varchar(6) DEFAULT NULL,
  `oper_date` datetime DEFAULT NULL,
  `send_date` datetime DEFAULT NULL,
  `mess` text,
  `region` varchar(1) DEFAULT NULL,
  `mess_id` decimal(20,0) DEFAULT NULL,
  `memo` varchar(100) DEFAULT NULL,
  `cust_name` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`flow_id`),
  KEY `i_bd_supcust_item_1` (`group_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_ms_mess_flow`
--

LOCK TABLES `t_ms_mess_flow` WRITE;
/*!40000 ALTER TABLE `t_ms_mess_flow` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_ms_mess_flow` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_st_material_templet`
--

DROP TABLE IF EXISTS `t_st_material_templet`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_st_material_templet` (
  `ch_templetname` varchar(30) NOT NULL,
  `ch_materialno` varchar(7) NOT NULL,
  PRIMARY KEY (`ch_materialno`,`ch_templetname`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_st_material_templet`
--

LOCK TABLES `t_st_material_templet` WRITE;
/*!40000 ALTER TABLE `t_st_material_templet` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_st_material_templet` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_sys_system`
--

DROP TABLE IF EXISTS `t_sys_system`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_sys_system` (
  `sys_var_id` varchar(20) NOT NULL,
  `sys_var_name` varchar(40) NOT NULL,
  `sys_var_value` varchar(80) DEFAULT NULL,
  `is_changed` varchar(2) DEFAULT NULL,
  `sys_var_desc` varchar(100) DEFAULT NULL,
  `sys_ver_flag` varchar(1) DEFAULT NULL,
  `sys_inited` varchar(1) DEFAULT NULL,
  `memo` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`sys_var_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_sys_system`
--

LOCK TABLES `t_sys_system` WRITE;
/*!40000 ALTER TABLE `t_sys_system` DISABLE KEYS */;
INSERT INTO `t_sys_system` VALUES ('Is_Distributed','系统变量Is_Distributed','1','否',NULL,'1',NULL,NULL),('pos_enter_new','系统变量 pos_enter_new','C<7:7141..',NULL,NULL,NULL,NULL,NULL),('pos_validate','系统变量 pos_validate','C<7:7141..',NULL,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `t_sys_system` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-11-13 11:45:19
